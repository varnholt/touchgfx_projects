"""Keep application-owned filenames and references lowercase after generation."""

from pathlib import Path
import re
import stat


ROOT = Path(__file__).resolve().parents[1]
GUI = ROOT / "TouchGFX" / "gui"
TEXT_SUFFIXES = {".cpp", ".hpp", ".h", ".c", ".vcxproj", ".filters",
                 ".ewp", ".uvprojx", ".uvoptx", ".project", ".cproject", ".mk"}


def normalize():
    owned = list(GUI.rglob("*"))
    owned.extend(p for p in (ROOT / 'TouchGFX' / 'target').glob('*')
                 if p.name.lower().startswith('stm32touchcontroller.'))
    basenames = {p.name.lower() for p in owned if p.is_file()}
    changed = 0
    for path in ROOT.rglob("*"):
        if not path.is_file() or path.suffix.lower() not in TEXT_SUFFIXES:
            continue
        if any(part in {"build", "Middlewares", "Drivers", ".git"}
               for part in path.relative_to(ROOT).parts):
            continue
        original = path.read_bytes()
        text = original.decode("utf-8-sig")
        for name in ('STM32TouchController.hpp', 'STM32TouchController.cpp'):
            text = text.replace(name, name.lower())
        # Never match gui_generated paths or ST/framework includes.
        text = re.sub(r'(\bgui[/\\](?:include[/\\]gui[/\\]|src[/\\])?[^\s<>";]+)',
                      lambda match: match[0].lower(), text)
        if GUI in path.parents:
            text = re.sub(r'(#include\s*["<])([^/\\"<>]+)([">])',
                          lambda m: m[1] + (m[2].lower() if m[2].lower() in basenames else m[2]) + m[3],
                          text)
        updated = text.encode("utf-8")
        if original.startswith(b"\xef\xbb\xbf"):
            updated = b"\xef\xbb\xbf" + updated
        if updated != original:
            # Designer marks generated sources read-only, including their includes.
            mode = path.stat().st_mode
            path.chmod(mode | stat.S_IWRITE)
            path.write_bytes(updated)
            path.chmod(mode)
            changed += 1
    for path in owned:
        if path.is_file() and path.name != path.name.lower():
            temporary = path.with_name(path.name + ".rename-tmp")
            path.rename(temporary)
            temporary.rename(path.with_name(path.name.lower()))
            changed += 1
    print(f"Normalized {changed} application files/references.")


if __name__ == "__main__":
    normalize()
