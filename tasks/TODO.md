# DreamCompiler TODO

This file tracks current development tasks and progress.

## Current Tasks

The November 3, 2026 baseline builds with Zig 0.17.1 and reports
110 passing Dream tests, 19 skipped, and no failures or errors. See
`MostRecentTestResults.md` for commands, limits, and platform verification.

1. **Daily-agent workflow** - Establish root instructions, durable pass records, and a reusable scheduled prompt; commit and push each completed pass.
2. **Optimization output** - Connect optimized IR to code generation and restore validated SSA construction.
3. **Coverage gaps** - Define expectations for the 19 skipped fixtures and repair the legacy Go suites.
4. **Deferred features** - Keep async and graphics experiments outside the supported baseline until implemented.

The inheritance pass now resolves declarations and preserves base subobjects,
typed fields, hidden fields, inherited methods/static members, base calls, and upcasts.
Function imports now work across source files; exporting types/globals and separate
module object caching remain outside that implemented subset.

## Feature Status

See `codex/FEATURES.md` for current implementation status of Dream language features.

## Development Guidelines

- Use `python codex/test_cli.py run --pattern "basics/arithmetic/*.dr"` for a small focused suite
- Use `zig build test` or `python codex/test_cli.py run` for the full Dream suite
- Tests in `not_implemented_tests/` directory are excluded from regular testing
- Keep this file updated with active development tasks only
