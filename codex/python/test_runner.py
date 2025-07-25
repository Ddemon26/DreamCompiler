#!/usr/bin/env python3
"""DreamCompiler regression test runner with verbose diagnostics."""

import argparse
import logging
import os
import platform
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
TEST_DIR = ROOT / "tests"
RUNTIME_DIR = ROOT / "runtime"


def run_cmd(cmd: list[str], *, cwd: Path = ROOT) -> subprocess.CompletedProcess:
    """Run a command and emit detailed logs."""
    logging.debug("Running command: %s", " ".join(cmd))
    logging.debug("Working directory: %s", cwd)
    res = subprocess.run(cmd, cwd=cwd, text=True, capture_output=True)
    logging.debug("Return code: %s", res.returncode)
    if res.stdout:
        logging.debug("stdout:\n%s", res.stdout)
    if res.stderr:
        logging.debug("stderr:\n%s", res.stderr)
    return res

def run_test(path: Path) -> bool:
    """Compile and execute a single test file with runtime library."""
    logging.info("Testing %s", path)
    text = path.read_text()
    matches = re.findall(r"//\s*Expected:\s*(.*)", text)
    opts = re.findall(r"//\s*Options:\s*(.*)", text)
    if not matches:
        logging.info("[SKIP] %s", path)
        return True

    parts = []
    for m in matches:
        m = m.strip()
        if m == "(no output)":
            continue
        parts.append(m)
    expected = "\n".join(parts)

    # Ensure compiler and runtime library are built
    if run_cmd(["zig", "build"]).returncode != 0:
        logging.error("[FAIL] %s (build failed)", path)
        return False

    compiler = ROOT / "zig-out" / "bin" / "DreamCompiler"
    if platform.system() == "Windows":
        compiler = compiler.with_suffix(".exe")

    # Generate C code only
    cmd = [str(compiler), "--dev"]
    if opts:
        cmd.extend(opts[0].split())
    cmd.append(str(path))
    if run_cmd(cmd).returncode != 0:
        logging.error("[FAIL] %s (compile error)", path)
        return False

    exe = "dream.exe" if platform.system() == "Windows" else "dream"
    exe_path = ROOT / exe
    lib = ROOT / "zig-out" / "lib" / "libdreamrt.a"

    # Compile generated C code with runtime library
    cc_cmd = [
        "zig",
        "cc",
        "-Isrc/runtime",
        "build/bin/dream.c",
        str(lib),
        "-o",
        str(exe_path),
    ]
    if run_cmd(cc_cmd).returncode != 0:
        logging.error("[FAIL] %s (link error)", path)
        return False

    res2 = run_cmd([str(exe_path)])
    if res2.returncode != 0:
        logging.error("[FAIL] %s (runtime error)", path)
        return False

    output = res2.stdout.strip()
    if output == expected:
        logging.info("[PASS] %s", path)
        return True

    logging.error(
        "[FAIL] %s (expected '%s', got '%s')",
        path,
        expected,
        output,
    )
    return False

def main() -> None:
    parser = argparse.ArgumentParser(description="Run DreamCompiler tests")
    parser.add_argument(
        "--filter",
        default="**/*.dr",
        help="glob pattern selecting which tests to run",
    )
    parser.add_argument(
        "--debug",
        action="store_true",
        help="enable verbose debug logging",
    )

    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG if args.debug else logging.INFO,
        format="%(levelname)s: %(message)s",
    )

    logging.debug("Python %s", sys.version.replace("\n", ""))
    logging.debug("Platform: %s", platform.platform())
    logging.debug("ROOT: %s", ROOT)
    logging.debug("TEST_DIR: %s", TEST_DIR)
    logging.debug("PATH: %s", os.environ.get("PATH"))
    logging.debug(
        "Runtime libs: %s",
        ", ".join(p.name for p in RUNTIME_DIR.glob("*")),
    )

    tests = sorted(TEST_DIR.glob(args.filter))
    if not tests:
        logging.warning("No tests found for pattern: %s", args.filter)

    ok = True
    for t in tests:
        if not run_test(t):
            ok = False
    if not ok:
        sys.exit(1)

if __name__ == '__main__':
    main()