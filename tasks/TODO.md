# DreamCompiler TODO

This file tracks current development tasks and progress.

## Current Tasks

The November 3, 2026 baseline builds with Zig 0.17.1 and reports
107 passing Dream tests, 22 skipped, and no failures or errors. See
`MostRecentTestResults.md` for commands, limits, and platform verification.

1. **Multi-file compilation** - Connect the driver, module emission, signatures, and linking; test real imports.
2. **Optimization output** - Connect optimized IR to code generation and restore validated SSA construction.
3. **Coverage gaps** - Define expectations for the 22 skipped fixtures and repair the legacy Go suites.
4. **Deferred features** - Keep async and graphics experiments outside the supported baseline until implemented.

The inheritance pass now resolves declarations and preserves base subobjects,
typed fields, hidden fields, inherited methods/static members, base calls, and upcasts.

## Feature Status

See `codex/FEATURES.md` for current implementation status of Dream language features.

## Development Guidelines

- Use `python codex/test_cli.py run --pattern "basics/arithmetic/*.dr"` for a small focused suite
- Use `zig build test` or `python codex/test_cli.py run` for the full Dream suite
- Tests in `not_implemented_tests/` directory are excluded from regular testing
- Keep this file updated with active development tasks only
