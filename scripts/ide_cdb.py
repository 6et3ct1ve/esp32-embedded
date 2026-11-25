"""Merge the clang-ready compile DBs of all projects into one at the repo root, for IntelliSense."""
import json
import subprocess
import sys
from pathlib import Path

root = Path(__file__).resolve().parent.parent
merged = []
for ini in sorted(root.glob("[0-9][0-9]_*/platformio.ini")):
    project = ini.parent
    tidy_dir = project / ".pio" / "tidy"
    subprocess.run(["pio", "run", "-s", "-t", "compiledb", "-d", str(project)], check=True)
    subprocess.run([sys.executable, str(root / "scripts" / "tidy_cdb.py"),
                    str(project / "compile_commands.json"), str(tidy_dir)], check=True)
    merged += json.loads((tidy_dir / "compile_commands.json").read_text())

(root / "compile_commands.json").write_text(json.dumps(merged, indent=1))
print(f"{len(merged)} entries -> compile_commands.json")
