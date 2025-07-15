Dream Compiler CLI Agent
========================

Overview
--------
The Dream Compiler CLI agent iteratively develops and maintains the Dream compiler.
The compiler is written in C and built using Zig. The agent runs in a Linux
terminal environment and uses `zig build` for compilation. Test files under
`tests/` are executed with `zig build run -- <file.dr>`.

Responsibilities
---------------
- Expand `src/` with new language features.
- Keep documentation in `docs/` up to date for every feature.
- Maintain matching test cases in `tests/` and ensure they pass.
- Record notable changes in `docs/changelog.md`.
- Add any required system dependencies to `codex/_startup.sh`.
- Break down large or monolithic source files into smaller modules when practical.

Workflow
--------
When the command `go` is issued the agent should:
1. Inspect the current compiler implementation and documentation.
   - Look for large modules that could be split into focused files before adding new features.
2. Implement the next logical feature or fix.
3. Update docs and tests accordingly.
4. Build with `zig build` and run all tests using `zig build run -- <test>`.
5. Commit the changes once tests succeed.

Environment
-----------
- Linux shell with Git and Zig available.
- `codex/_startup.sh` installs packages via `apt` (e.g. `build-essential` and `zig`).

Directory Structure
-------------------
- `src/`      – Compiler source code.
- `docs/`     – Markdown documentation for language features and usage.
- `tests/`    – Dream source files used as regression tests.
- `codex/`    – Contains this `AGENTS.md` and the `_startup.sh` script.

Initial Features
----------------
The compiler currently supports:
- Integer variables and assignments.
- Addition expressions.
- `Console.WriteLine` for output.
- Basic `if` statements.

Future work includes loops, functions and string output.

Example Execution
-----------------
```
> go
[Agent] Building compiler...
[Agent] Running tests...
[Agent] All tests passed!
[Agent] Committing changes...
```

Pull Request Template
---------------------
All pull request descriptions should follow `codex/BOT_PR_TEMPLATE.md`. Fill out each section of that template to describe the changes, tests, documentation updates and any dependency modifications.
