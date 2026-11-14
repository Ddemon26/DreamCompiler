# Building DreamCompiler

Use Zig **0.17.1** and re2c on `PATH`. Python is needed for `zig build test`;
Go is needed for the separate CLI stability checks. Build from the repository
root. See [build.zig](../../../build.zig) for the actual build graph and
[README](../../../README.md) for setup.

## Build and run

```sh
zig version
zig build
zig build run -- tests/basics/io/console.dr
```

The compiler is installed at `zig-out/bin/DreamCompiler` (`.exe` on Windows).
The last command compiles a Dream source; run its generated program separately:

```sh
./dream
```

Use `.\dream.exe` on Windows. Ordinary compilation writes `build/bin/dream.c`
and links the program with `zig-out/lib/dreamrt`. It does not name the executable
after the source file. Generated C, objects, binaries and caches are build outputs.

## Bootstrap and source generation

The C-only host executable `dreamc-bootstrap` translates `.dr` files under `src/`
into cached C library translation units. The production compiler combines those
with its C sources and the runtime. Entry-file newline normalization is the first
production Dream helper. The bootstrap includes a C seed for this function;
the production compiler excludes the seed. See the
[dogfooding contract](../compiler-dogfooding.md) for the stages and
`--emit-library-c OUTPUT SOURCE` interface.

re2c generates the lexer from `src/lexer/lexer.re` and `tokens.def` during the
build. Edit those inputs, not the generated lexer. The source map identifies the
[working implementation](../source-map.md).

## Supported build options and steps

```sh
zig build -Doptimize=ReleaseSafe
zig build -Doptimize=ReleaseFast
zig build -Doptimize=ReleaseSmall
zig build -Ddebug=true
zig build test
zig build compile-multi -- tests/multifile/main.dr
```

`-Doptimize` controls the toolchain's build optimization. It is separate from the
Dream compiler's `-O1`/`-O2`/`-O3` flags. `zig build debug` depends on the compiler
build; use `-Ddebug=true` to select its debug defines. `zig build format` runs
formatters and changes source files; it is not a read-only verification command.
`compile-dr` generates the compiler's Dream helper C without installing all tools.
The optional Vulkan SDK enables experimental graphics build inputs; its presence
does not qualify graphics runtime behavior.

## Cross-builds

```sh
# Windows host to Linux
zig build -Dtarget=x86_64-linux-gnu --prefix build/linux
# Linux host to Windows
zig build -Dtarget=x86_64-windows-gnu --prefix build/windows
```

The bootstrap always runs on the host. The production compiler, runtime and other
tools target the requested platform. Cross-linking is separate from native test
execution. Consult [testing](../../../codex/TEST_GUIDE.md) for required checks and
[the workflow](../agent-workflow.md) for delivery requirements.
