#include "gui/common/poly.hpp"
#include "gui/common/vector.hpp"
#include "gui/common/torus.hpp"
#include "gui/common/smokegrid.hpp"
#include "gui/common/touchinput.hpp"
#include "gui/common/touchfilter.hpp"
#include "gui/common/cameraprojection.hpp"
#include "gui/common/smokesimulation.hpp"

#include <algorithm>
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>

namespace
{
constexpr int frameWidth = 8;
constexpr int frameHeight = 8;
constexpr int guard = 512;
constexpr unsigned short sentinel = 0xa55a;
constexpr unsigned short color = 0x1234;
std::array<unsigned short, 256 * 256> texture;

void require(bool condition, const char* message)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

void renderGuarded(Vertex a, Vertex b, Vertex c, bool compareReference)
{
    std::array<unsigned short, guard + frameWidth * frameHeight + guard> buffer;
    buffer.fill(sentinel);
    std::fill(buffer.begin() + guard, buffer.begin() + guard + frameWidth * frameHeight, static_cast<unsigned short>(0));
    drawTriangle(buffer.data() + guard, &a, &b, &c, texture.data(), frameWidth, frameHeight);
    require(std::all_of(buffer.begin(), buffer.begin() + guard,
                       [](unsigned short v) { return v == sentinel; }), "write before framebuffer");
    require(std::all_of(buffer.begin() + guard + frameWidth * frameHeight, buffer.end(),
                       [](unsigned short v) { return v == sentinel; }), "write after framebuffer");
    if (compareReference)
    {
        std::array<unsigned short, 64 * 64> reference = {};
        a.x += 24; a.y += 24;
        b.x += 24; b.y += 24;
        c.x += 24; c.y += 24;
        drawTriangle(reference.data(), &a, &b, &c, texture.data(), 64, 64);
        for (int y = 0; y < frameHeight; ++y)
            for (int x = 0; x < frameWidth; ++x)
                require(buffer[guard + y * frameWidth + x] == reference[(y + 24) * 64 + x + 24],
                        "clipped triangle differs from cropped full triangle");
    }
}
}

void runHardwareTouchTests();

int main()
{
    texture.fill(color);
    // Flat top: all of the rendering occurs in the formerly unclipped bottom half.
    renderGuarded({-8, -2, 0, 0, 0}, {-8, 16, 0, 0, 0}, {16, -2, 0, 0, 0}, true);
    renderGuarded({0, 0, 0, 0, 0}, {0, 8, 0, 0, 0}, {16, 0, 0, 0, 0}, true);
    renderGuarded({-12, -12, 0, 0, 0}, {-12, -4, 0, 0, 0}, {-4, -12, 0, 0, 0}, true);

    std::array<unsigned short, frameWidth * frameHeight> pixels = {};
    Vertex a = {0, 0, 0, 0, 0}, b = {0, 8, 0, 0, 0}, c = {16, 0, 0, 0, 0};
    drawTriangle(pixels.data(), &a, &b, &c, texture.data(), frameWidth, frameHeight);
    require(pixels[frameWidth - 1] == color, "rightmost framebuffer column omitted");

    pixels.fill(0);
    b = {4, 4, 0, 0, 0}; c = {8, 8, 0, 0, 0};
    std::feclearexcept(FE_ALL_EXCEPT);
    drawTriangle(pixels.data(), &a, &b, &c, texture.data(), frameWidth, frameHeight);
    require(std::fetestexcept(FE_DIVBYZERO | FE_INVALID) == 0, "degenerate triangle arithmetic");
    require(std::all_of(pixels.begin(), pixels.end(), [](unsigned short v) { return v == 0; }),
            "degenerate triangle draws pixels");

    std::mt19937 random(42);
    std::uniform_int_distribution<int> coord(-12, 20);
    for (int i = 0; i < 5000; ++i)
    {
        a = {static_cast<float>(coord(random)), static_cast<float>(coord(random)), 0, 0, 0};
        b = {static_cast<float>(coord(random)), static_cast<float>(coord(random)), 0, 0, 0};
        c = {static_cast<float>(coord(random)), static_cast<float>(coord(random)), 0, 0, 0};
        renderGuarded(a, b, c, false);
    }

    Vector3D zero;
    zero.normalize();
    require(zero.x == 0 && zero.y == 0 && zero.z == 0, "default/normalized zero vector");
    Vector3D normal(3, 4, 0);
    normal.normalize();
    require(std::fabs(normal * normal - 1.0f) < 0.00001f, "unit normal length");

    using Grid = SmokeGrid<80>;
    require(Grid::inputIndex(0, 0, 800, 480) == Grid::index(0, 0), "first smoke cell mapping");
    require(Grid::inputIndex(799, 479, 800, 480) == Grid::index(79, 79), "last smoke cell mapping");
    require(Grid::inputIndex(800, 0, 800, 480) == -1 && Grid::inputIndex(-1, 0, 800, 480) == -1 &&
            Grid::inputIndex(0, 480, 800, 480) == -1 && Grid::inputIndex(0, -1, 800, 480) == -1,
            "out-of-widget smoke input");
    for (int y = 0; y < 480; ++y)
        for (int x = 0; x < 800; ++x)
        {
            const int index = Grid::inputIndex(x, y, 800, 480);
            require(index % 82 >= 1 && index % 82 <= 80 && index / 82 >= 1 && index / 82 <= 80,
                    "smoke input must never target boundary padding");
        }

    Torus<8, 6> mesh(12, 5);
    static_assert(sizeof(mesh.triIndices) / sizeof(int32_t) == 8 * 6 * 2 * 3,
                  "mesh must allocate exactly three indices per triangle");
    for (int i = 0; i < mesh.getNumTris() * 3; ++i)
        require(mesh.triIndices[i] >= 0 && mesh.triIndices[i] < mesh.getNumVertices(), "mesh index range");
    // A partial redraw must preserve all pixels outside its rectangle, including row padding.
    std::array<uint16_t, 24 * 18> padded;
    padded.fill(sentinel);
    const RasterSurface partial(padded.data(), 20, 18, 24, 3, 2, 8, 8, {2, 1, 6, 7});
    a = {-100, -100, 0, 0, 0}; b = {-100, 200, 0, 0, 0}; c = {200, -100, 0, 0, 0};
    drawSolidTriangle(partial, a, b, c, color);
    for (int y = 0; y < 18; ++y)
        for (int x = 0; x < 24; ++x)
            require(padded[y * 24 + x] == ((x >= 5 && x < 9 && y >= 3 && y < 9) ? color : sentinel),
                    "partial redraw, widget offset or padded stride");
    padded.fill(sentinel);
    const RasterSurface offscreen(padded.data(), 20, 18, 24, -3, -2, 8, 8, {0, 0, 8, 8});
    drawTriangle(offscreen, a, b, c, texture.data());
    for (int y = 0; y < 18; ++y)
        for (int x = 0; x < 24; ++x)
            require(padded[y * 24 + x] == ((x < 5 && y < 6) ? color : sentinel), "offscreen widget clipping");

    TouchInput touch;
    touch.press(20, 30, 800, 480);
    touch.release();
    require(touch.consume().pressed, "a short tap must reach the next tick");
    require(!touch.consume().pressed, "release must not add a second press");
    touch.press(20, 30, 800, 480);
    touch.drag(20, 30, 25, 35, 800, 480);
    touch.drag(25, 35, 32, 42, 800, 480);
    auto motion = touch.consume();
    require(motion.dx == 12 && motion.dy == 12 && motion.x == 32, "coalesced drag must preserve movement");
    touch.cancel();
    touch.drag(32, 42, 40, 50, 800, 480);
    motion = touch.consume();
    require(!motion.pressed && motion.dx == 0 && motion.dy == 0, "cancelled stroke must not inject input");
    touch.press(10, 10, 800, 480);
    touch.consume();
    touch.drag(10, 10, -1, 10, 800, 480);
    touch.drag(-1, 10, 20, 10, 800, 480);
    require(touch.consume().dx == 0, "reentry must not create a jump from an outside coordinate");

    TouchFilter filter;
    int filteredX = 0, filteredY = 0;
    require(filter.sample(true, 2, 2, 800, 480, filteredX, filteredY) && filteredX == 2 && filteredY == 2,
            "first touch near origin must not be suppressed by the jitter threshold");
    filter.sample(true, 3, 3, 800, 480, filteredX, filteredY);
    require(filteredX == 2 && filteredY == 2, "jitter filter within one contact");
    filter.sample(false, 0, 0, 800, 480, filteredX, filteredY);
    filter.sample(true, 3, 3, 800, 480, filteredX, filteredY);
    require(filteredX == 3 && filteredY == 3, "new contact must not inherit the old jitter anchor");
    require(!filter.sample(true, 800, 0, 800, 480, filteredX, filteredY), "reject corrupt hardware coordinates");

    CameraVertex cameraTriangle[] = {{Vector3D(-1, 0, 0.5f), 0, 0},
                                     {Vector3D(0, 1, 2), 65536, 0}, {Vector3D(1, 0, 2), 0, 65536}};
    CameraVertex clipped[4];
    const int clippedCount = clipNearPlane(cameraTriangle, clipped, 1.0f);
    require(clippedCount == 4, "near-plane crossing triangle must become a quad");
    for (int i = 0; i < clippedCount; ++i)
    {
        require(clipped[i].position.z >= 1.0f, "near-plane output depth");
        const auto projected = projectCameraVertex(clipped[i], 800, 480);
        require(std::isfinite(projected.x) && std::isfinite(projected.y), "near-plane projection must be finite");
    }
    for (auto& vertex : cameraTriangle) vertex.position.z = -1;
    require(clipNearPlane(cameraTriangle, clipped, 1.0f) == 0, "triangle behind the camera must be rejected");

    SmokeSimulation simulation;
    simulation.input().press(400, 240, 800, 480);
    simulation.input().release();
    simulation.step(800, 480);
    const float tapDensity = simulation.density(40, 40);
    require(tapDensity > 0, "short tap must add smoke");
    simulation.step(800, 480);
    require(std::fabs(simulation.density(40, 40) - tapDensity) < 0.00001f,
            "release must not inject a second puff");
    simulation.reset();
    simulation.input().press(400, 240, 800, 480);
    simulation.input().cancel();
    simulation.step(800, 480);
    require(simulation.density(40, 40) == 0, "cancelled tap must not add smoke");
    runHardwareTouchTests();
    std::cout << "PASS: 5000 guarded triangles; clipping, stride, offsets, vectors, mesh, touch lifecycle, hardware adapter with I2C fault injection, near plane, smoke simulation\n";
}
