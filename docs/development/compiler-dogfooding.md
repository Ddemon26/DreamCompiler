# Compiler dogfooding checkpoint

P0025 starts production compiler dogfooding with
[`src/driver/source_text.dr`](../../src/driver/source_text.dr). Every entry file
passes through its `dream_normalize_newlines(char* buffer)` before parsing. It
collapses CRLF to LF in place, preserves lone CR and all other nonzero bytes, and
stops at NUL. The caller supplies writable NUL-terminated storage. The function
borrows that storage, does not allocate, and uses pointer cursors rather than
introducing a narrower integer length limit. This preserves the previous C
implementation's single-pass behavior, including CR CR LF becoming CR LF.

## Build stages

Run `zig build` with Zig 0.17.1 and re2c from the project root:

1. Build the C-only host executable `dreamc-bootstrap`, including
   `src/driver/source_text_bootstrap.c` as its normalization seed.
2. Run that executable to translate Dream files under `src/` into cached C library
   translation units. Input files and generated outputs are build dependencies;
   generated C is not checked in.
3. Compile the regular C sources and generated Dream helpers for the requested
   target, link the runtime, and install `DreamCompiler`. The production compiler
   excludes `source_text_bootstrap.c`; its entry-file normalizer comes from Dream.

Both compiler executables are installed under the selected prefix's `bin/`.
The bootstrap always targets the build host, including cross-builds such as
`zig build -Dtarget=x86_64-linux-gnu --prefix build/linux` on Windows. It must run
locally to generate target-independent C; the production compiler targets Linux.
`lexdump` and `parse` remain C tools. Dependency loading remains in C.

## Function-only C library emission

```sh
./zig-out/bin/DreamCompiler --emit-library-c build/source_text.c src/driver/source_text.dr
```

Use `.exe` on Windows. Create the output directory first. This mode runs the real
parser, semantic analysis and common AST emitter, writes only the named C file,
and does not invoke the host compiler or write `build/bin/dream.c`. It exposes
functions with external C linkage and emits no program entry point. Calls between
functions retain forward prototypes. Ordinary program and object emission retain
their existing linkage and entry behavior.

The mode accepts synchronous function declarations without `main`. It rejects
top-level statements, global storage, classes/structs, modules and async functions;
`--multi-file` and `--emit-obj` cannot be combined with it. Rejected source does
not create or overwrite the requested output. Output-open/write errors fail.

Compile the result as a separate C translation unit with the runtime header
directories (`src/runtime/io`, `extensions`, `memory`, `exceptions`, `system`) on
the include path, and link `dreamrt`. The embedding host owns any runtime
initialization and shutdown needed by its functions. This is a narrow compiler
integration boundary, not a general C FFI or new ownership contract. Existing
return, pointer-shape and lifetime limitations still apply.

## Evidence and next milestones

`TestCompiler_Dogfood` runs in both Go quick protocols and CI. It checks external
pointer signatures, absence of an entry point, identical helper C emitted by the
C seed and production compiler, artifact-free rejections, preservation of existing
output, invalid output requests, and production entry files with LF/CRLF/CRCRLF.
The separately linked C harness compares the Dream helper against the C seed and
independent expected output across 97,912 inputs: all sequences of up to seven
bytes from CR/LF/ASCII/high-byte values, every nonzero single byte, and embedded
NUL. It checks whole-buffer seed equivalence, boundary sentinels and repeated-call
equivalence. Windows additionally cross-links the harness for Linux; native Linux
runtime evidence comes from CI.

This is one compiler helper written in Dream. The lexer, parser, semantics and C
emitter remain C. Full self-hosting needs compiler data structures, custom-type
interfaces, shared types across modules, qualified ownership and system access,
followed by migrating the frontend/emitter and verifying successive compiler
builds. The C backend and Zig toolchain can remain. See the
[backlog](../../tasks/TODO.md) and [P0025 record](../../tasks/passes/P0025-compiler-dogfooding.md).
