#!/usr/bin/env python3
"""Verify that generated C includes #line directives for the given source."""
import subprocess
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parent.parent.parent

if len(sys.argv) != 2:
    print("usage: check_line_directives.py <source.dr>")
    sys.exit(1)

src = Path(sys.argv[1])
res = subprocess.run(["zig", "build", "run", "--", str(src)], cwd=ROOT)
if res.returncode != 0:
    sys.exit(res.returncode)

c_file = ROOT / "build" / "bin" / "dream.c"
text = c_file.read_text()
count = text.count(f'"{src.as_posix()}"')
print(f"found {count} line directives for {src}")
if count < 3:
    print(text)
    sys.exit(1)
