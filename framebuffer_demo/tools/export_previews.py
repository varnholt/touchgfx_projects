"""Export unmodified simulator framebuffer captures to the project gallery."""
from pathlib import Path
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
for screen in ("home", "torus", "smoke", "shapes"):
    source = ROOT / "TouchGFX" / "build" / "Release" / "bin" / f"{screen}.bmp"
    Image.open(source).save(ROOT / "doc" / f"{screen}.png")
print("Exported Home, Torus, Smoke and Shapes previews.")
