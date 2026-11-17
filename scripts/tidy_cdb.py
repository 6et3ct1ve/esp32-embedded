"""Rewrite a PlatformIO compile_commands.json (Xtensa GCC) so clang-tidy can parse it."""
import json, os, shlex, shutil, subprocess, sys
from pathlib import Path

GCC_ONLY = {"-mlongcalls", "-mtext-section-literals", "-fstrict-volatile-bitfields",
            "-fno-tree-switch-conversion", "-freorder-blocks", "-fipa-pta", "-free"}

def resolve(gcc):
    # PlatformIO writes a bare compiler name; its toolchains live in ~/.platformio/packages.
    pio_bins = Path(os.environ.get("PLATFORMIO_CORE_DIR", Path.home() / ".platformio")).glob("packages/toolchain-*/bin")
    found = shutil.which(gcc, path=os.pathsep.join([*map(str, pio_bins), os.environ.get("PATH", "")]))
    if not found:
        sys.exit(f"compiler not found: {gcc}")
    return found

def system_includes(gcc, lang):
    out = subprocess.run([gcc, f"-x{lang}", "-E", "-v", "-"], input="", capture_output=True, text=True).stderr
    block = out.split("#include <...> search starts here:")[1].split("End of search list.")[0]
    return [str(Path(p.strip()).resolve()) for p in block.strip().splitlines()]

def fix(args, is_c):
    gcc, rest = resolve(args[0]), args[1:]
    keep = []
    skip = False
    for a in rest:
        if skip: skip = False; continue
        if a == "-o": skip = True; continue
        if a in GCC_ONLY or a.startswith("-mfix-esp32-psram-cache"): continue
        keep.append(a)
    target = ["--target=i686-unknown-none-elf", "-U__i386__", "-D__XTENSA__=1", "-D__XTENSA_EL__=1", "-D__XTENSA_WINDOWED_ABI__=1",
              "-mlong-double-64", "-funsigned-char", "-fshort-wchar",
              "-nostdinc"] + ([] if is_c else ["-nostdinc++"])
    sysinc = [x for p in system_includes(gcc, "c" if is_c else "c++") for x in ("-isystem", p)]
    return ["clang" if is_c else "clang++"] + target + keep + sysinc

src = Path(sys.argv[1]); dst = Path(sys.argv[2])
project = src.resolve().parent
# Keep only the project's own sources; framework files are not ours to lint.
db = [e for e in json.loads(src.read_text())
      if (Path(e["directory"]) / e["file"]).resolve().is_relative_to(project)]
for e in db:
    args = e.pop("arguments", None) or shlex.split(e.pop("command"))
    e["arguments"] = fix(args, e["file"].endswith(".c"))
dst.mkdir(parents=True, exist_ok=True)
(dst / "compile_commands.json").write_text(json.dumps(db, indent=1))
print(f"{len(db)} entries -> {dst/'compile_commands.json'}")
