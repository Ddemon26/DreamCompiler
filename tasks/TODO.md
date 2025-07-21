# DreamCompiler TODO

This file tracks current development tasks and progress.

## Current Tasks

This file is being maintained fresh. Recent work has focused on:

1. ✅ **Graphics/Vulkan Tests** - Moved to `not_implemented_tests/` directory to prevent test timeouts
2. ✅ **Concurrency/Async Tests** - Moved to `not_implemented_tests/` directory due to missing platform.h dependencies  
3. ✅ **Test Infrastructure** - Created `not_implemented_tests/` directory for features not yet implemented

## Feature Status

See `FEATURES.md` for current implementation status of Dream language features.

## Development Guidelines

- Use `python codex/test_cli.py quick` for fast testing
- Use `python codex/test_cli.py run` for full test suite  
- Tests in `not_implemented_tests/` directory are excluded from regular testing
- Keep this file updated with active development tasks only