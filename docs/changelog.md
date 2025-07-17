# Changelog

All notable changes to the Dream compiler will be documented in this file.

## [Unreleased]
- Added automated test runner to `zig build test`.
- Implemented parsing of `if`/`else` statements.
- Lexer updated to recognise additional tokens (`??`, `??=`, `=>`, `::`, `->`,
  `&=`, `|=`, `^=`, `<<=`, `>>=`) per Grammar v0.3.
- Added basic C code generation for programs.
- Implemented `--emit-obj` to compile generated C to an object file.
- Recognised `void` as reserved keyword per Grammar v0.3.
- Fixed Windows build by replacing `mkstemps` with portable `tmpnam_s` fallback.
- Restored generation of `build/bin/dream.c` when compiling `.dr` files.
