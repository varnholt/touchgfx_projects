# Review and modernization

Reviewed 2026-11-08. Scope: application GUI, custom renderers and simulator build.
The application touch adapter is tested against the bundled component driver with a mocked HAL. Physical touch hardware, interrupt timing and hardware rendering remain unvalidated.

## First fixes

| Issue | Impact | Change |
| --- | --- | --- |
| Textured triangles omitted horizontal clipping in their lower half | Writes outside the framebuffer when a triangle crosses a screen edge | Clip both halves; pass dimensions explicitly instead of using external globals |
| Both triangle renderers clipped an exclusive right edge to width minus one | Last display column omitted | Clip exclusive edge to width |
| Degenerate textured triangles divided by zero before rasterization | Invalid texture gradients and floating-point conversions | Reject zero/non-finite area |
| Default vectors left all components uninitialized | Shape centers and positions read an uninitialized z component during arithmetic | Initialize vectors to zero; handle zero-length normalization |
| Effects registered for timer ticks without unregistering on destruction | Unbalanced widget lifecycle and possible stale timer entries across view destruction | Add matching destructors |
| Smoke input added padding twice | Top/left cells skipped, bottom/right inputs placed into solver boundaries | Share zero-based-to-padded mapping and reject out-of-widget input |
| Invalid smoke input returned before processing background effects | Dragging outside the effect could suppress overlay/kickoff work | Consume input independently of background effects |
| Smoke reset retained pending interaction flags | Old impulses could survive reset | Clear touch, drag and kickoff flags |
| Smoke simulation and torus inertia advanced during draw | Simulation speed depended on redraw frequency | Update state once per tick |
| Mesh index arrays allocated six indices per triangle | 24,192 unnecessary bytes across the three torus meshes | Allocate three indices per triangle; replace leaking macros with scoped constants |
| Application filenames/includes used mixed casing | Portability and naming inconsistency | Lowercase application-owned GUI files and update references |

## Validation

Run `tests/run-tests.ps1` for native C++ regression tests covering:

- Framebuffer guards for 5,000 deterministic triangles.
- Clipped output against a crop of a larger reference framebuffer.
- Rightmost-column coverage and zero-area triangle rejection.
- Vector initialization and normalization.
- All 384,000 pixel positions mapping into smoke interior cells, plus outside edges.
- Torus triangle index capacity and index ranges.

The native test executable uses application code and the bundled FT6x06 driver
without linking the TouchGFX core library.
Debug and Release simulator integration checks are described below. These native
tests do not establish physical hardware correctness.

## Follow-up fixes

- All effects now honor invalidated rectangles through a shared RGB565 surface
  with widget offsets, framebuffer dimensions, pixel stride and a clip rectangle.
  Shared edges use consistent interpolation. The views use normal occlusion culling
  again; the full-screen SMOC workaround has been removed.
- The torus clips triangles at a near plane before projection, bounds drag-driven
  camera displacement and sorts faces in fixed-capacity storage using float depth.
  No map nodes are allocated during rendering, and scratch arrays no longer use
  const casts. Long-running animation phases are wrapped to retain float precision.
- Smoke simulation is a standalone model. Drawing reads that model and never
  advances it. Integer cell boundaries cover the complete widget for small and
  non-divisible sizes; all quality modes obey clipping. Texture data remains const.
- A shared pointer lifecycle distinguishes press, release and cancellation.
  Release cannot add a second puff, cancelled pending input is discarded, multiple
  drag events accumulate movement, and outside/reentry coordinates cannot generate
  a jump. Shapes ignore release/cancel positions, and all effects reject stray
  drags without an active press.
- The application-owned hardware adapter clears failed I2C read buffers, propagates
  read failure to the sample result and rejects out-of-range coordinates. Its
  position filter resets on release and accepts the first sample of each contact,
  including contacts close to the previous touch or the origin.

The rendering contract follows ST's
[custom widget documentation](https://support.touchgfx.com/docs/development/ui-development/touchgfx-engine-features/custom-widgets).
The effects support full, unrotated RGB565 framebuffers. Other formats, rotated
framebuffers and partial framebuffer strategies are explicitly rejected rather
than interpreted as RGB565. The generated screen layout and physical adapter still
use the original F769 800x480 board profile; flexible widget rendering does not
make the generated UI responsive or add other display/board support.

## Expanded validation

`tests/run-tests.ps1` additionally covers pointer lifecycle, coalesced drag motion,
release/cancel smoke behavior, camera near-plane clipping, padded framebuffer rows,
partial clip rectangles and widgets partly outside the framebuffer. The actual
hardware adapter and bundled FT6x06 component driver are compiled into the test
executable with a mocked HAL. I2C failure, recovery, invalid coordinates and
position-filter reset paths are exercised without changing the driver library.

`tests/run-tests.ps1 -Simulator -Configuration Debug` runs the real TouchGFX
framework in a self-test that exits after 48 screen transitions. It exercises all
quality modes and press/drag/release/cancel events, checks stable timer counts,
checks repeated drawing leaves the frame unchanged, and uses framebuffer sentinels
to detect writes outside partial invalidations. It checks 83x47 and 7x5 smoke
widgets in every quality mode with full and partial redraws, and captures actual
framebuffer snapshots. Smoke, torus and shapes snapshots have been inspected.
The same test supports `-Configuration Release`.

Remaining work is the official framework migration below, physical-board testing
of I2C fault recovery and touch feel, and rendering/memory/performance measurements
on the F769. The adapter still exposes the first contact through TouchGFX's
single-pointer interface; stable multi-contact identity tracking is not provided.

## TouchGFX upgrade target

ST lists **4.27.0**, released October 5, 2026, as the current release in its
[changelog](https://support.touchgfx.com/docs/miscellaneous/changelog).
This tree is **4.17.0**, based on CubeMX **6.2.1** and STM32CubeF7 **1.16.1**
according to the bundled `changelog.txt`. Newer TouchGFX versions added native
Visual Studio 2022 support and CMake project update support; the current build's
v142 library override is a compatibility bridge for 4.17, not the final setup.

Recommended sequence:

1. Preserve this baseline on a branch/tag before migrating. Keep custom source
   separate from generated files and retain the matching 4.17 dependency setup.
2. Install the official 4.27 Designer/Generator package and use the Designer's
   updater on a copy of `TouchGFX/framebuffer_demo.touchgfx`. Regenerate the GUI,
   images, fonts and texts together with the matching framework libraries/tools.
   ST describes its update flow in the
   [upgrade guide (archived 4.13)](https://support.touchgfx.com/4.13/docs/miscellaneous/updating-to-a-new-touchgfx-version/).
   Check intervening changes against the current
   [known issues](https://support.touchgfx.com/docs/miscellaneous/known-issues),
   including the transition from texts.xlsx to texts.xml.
3. Run `python tools/normalize_gui_filenames.py` after generation. Designer emits
   mixed-case references to application headers; the script fixes those references
   without renaming generated files or changing ST code. Recheck the simulator
   project import because Designer can rewrite its project file.
4. Replace the 4.17-specific simulator library override and Ruby asset conversion
   assumptions with the regenerated 4.27 build. Run regression tests and build
   both simulator configurations, then verify the screens interactively.
5. Upgrade Generator/CubeMX/CubeF7 coherently on a separate hardware branch/copy.
   Compare the current F769 board setup with an official setup available in the
   installed Designer; current exact template availability is not verified here.
   Validate SDRAM, QSPI assets, DSI/LTDC timing, DMA2D, MPU/cache configuration,
   FreeRTOS and external-loader programming on the physical board.

No ST library source or framework version has been changed. Hardware touch
changes are confined to the application-owned adapter, whose filenames and
references also follow the lowercase convention. A framework upgrade is not
complete until the official matching package is installed, generated output is
refreshed and hardware is tested.
