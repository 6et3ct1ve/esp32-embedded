"""Rewrite a PlatformIO compile_commands.json (Xtensa GCC) so clang-tidy can parse it."""
import json, os, shlex, shutil, subprocess, sys, tempfile
from functools import cache
from pathlib import Path

C_EXT = {".c"}
CXX_EXT = {".cc", ".cpp", ".cxx"}

# Same type sizes and macros as Xtensa GCC, on a 32-bit bare-metal target clang knows.
TARGET = ["--target=i686-unknown-none-elf", "-U__i386__",
          "-D__XTENSA__=1", "-D__XTENSA_EL__=1", "-D__XTENSA_WINDOWED_ABI__=1",
          "-mlong-double-64", "-funsigned-char", "-fshort-wchar",
          "-Wno-unknown-warning-option", "-nostdinc"]

@cache
def project_compilers(project):
    # PlatformIO knows the exact compiler of the project's toolchain. The compile DB only
    # has a bare name, and several toolchains (Arduino GCC 8, ESP-IDF GCC 15) share it.
    out = subprocess.run(["pio", "project", "metadata", "-d", str(project), "--json-output"],
                         capture_output=True, text=True, check=True).stdout
    meta = json.loads(next(line for line in reversed(out.splitlines()) if line.startswith("{")))
    return {Path(p).name: p for env in meta.values() for p in (env["cc_path"], env["cxx_path"]) if p}

def resolve(gcc, project):
    if os.path.isabs(gcc):
        return gcc
    found = project_compilers(project).get(Path(gcc).name)
    if not found:
        sys.exit(f"compiler not found: {gcc}")
    return found

@cache
def system_includes(gcc, lang, specs):
    # Specs files (e.g. picolibc in ESP-IDF 6) change the C library headers, so pass them too.
    out = subprocess.run([gcc, *specs, f"-x{lang}", "-E", "-v", "-"], input="", capture_output=True, text=True).stderr
    block = out.split("#include <...> search starts here:")[1].split("End of search list.")[0]
    return tuple(str(Path(p.strip()).resolve()) for p in block.strip().splitlines())

CLANG_TIDY = shutil.which("clang-tidy-18") or shutil.which("clang-tidy") or sys.exit("clang-tidy not found")

@cache
def clang_accepts(flag, lang):
    # GCC-only -f/-m flags make clang fail; ask clang (through clang-tidy, no separate
    # clang install needed) instead of keeping a list per GCC version.
    with tempfile.TemporaryDirectory() as tmp:
        probe = Path(tmp) / ("probe.c" if lang == "c" else "probe.cpp")
        probe.write_text("")
        r = subprocess.run([CLANG_TIDY, "--checks=-*", str(probe), "--", *TARGET, flag],
                           capture_output=True, text=True)
    return r.returncode == 0

def expand_response_files(args, directory):
    # CMake (ESP-IDF) passes flags and includes through @file response files.
    out = []
    for a in args:
        if a.startswith("@"):
            out += shlex.split((Path(directory) / a[1:]).read_text())
        else:
            out.append(a)
    return out

def fix(args, directory, is_c, project):
    lang = "c" if is_c else "c++"
    gcc, rest = resolve(args[0], project), expand_response_files(args[1:], directory)
    keep, skip = [], False
    specs = tuple(a for a in rest if a.startswith(("-specs=", "--specs=")))
    for a in rest:
        if skip:
            skip = False
            continue
        if a in specs:
            continue
        if a == "-o":
            skip = True
            continue
        if a.startswith(("-f", "-m")) and not clang_accepts(a, lang):
            continue
        keep.append(a)
    sysinc = [x for p in system_includes(gcc, lang, specs) for x in ("-isystem", p)]
    std_inc = [] if is_c else ["-nostdinc++"]
    return ["clang" if is_c else "clang++"] + TARGET + std_inc + keep + sysinc

src = Path(sys.argv[1]); dst = Path(sys.argv[2])
project = src.resolve().parent
db = []
for e in json.loads(src.read_text()):
    path = (Path(e["directory"]) / e["file"]).resolve()
    # Only the project's own C/C++ sources: no framework, build output or assembly.
    if not path.is_relative_to(project) or ".pio" in path.relative_to(project).parts:
        continue
    if path.suffix not in C_EXT | CXX_EXT:
        continue
    args = e.pop("arguments", None) or shlex.split(e.pop("command"))
    e["arguments"] = fix(args, e["directory"], path.suffix in C_EXT, project)
    db.append(e)
dst.mkdir(parents=True, exist_ok=True)
(dst / "compile_commands.json").write_text(json.dumps(db, indent=1))
print(f"{len(db)} entries -> {dst/'compile_commands.json'}")
