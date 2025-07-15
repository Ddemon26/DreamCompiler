Dream Compiler Pull Request

Description
Provide a concise summary of this pull request. Mention any new features, bug fixes, or improvements made to the Dream compiler. The compiler is written in C and built via Zig using `build.zig`.

Example:
- Added `while` loop support to the parser and code generator.
- Documented the feature in `docs/v1/loops.md`.
- Added `tests/advanced/while_loop.dr`.

Related Files
List the main files that were modified or added:
- `src/` for compiler changes
- `docs/` for documentation updates
- `tests/` for new or updated test cases
- `codex/_startup.sh` if dependencies changed

Example:
Modified: `src/parser.c`
Added: `docs/v1/loops.md`
Added: `tests/advanced/while_loop.dr`

Changes
Detail the implementation changes and why they were made. Include any updates to language syntax, parser logic, code generation, or documentation. Mention if system dependencies were added or removed.

Testing
Confirm that the compiler builds and all tests run without errors:
```bash
zig build
for f in tests/*/*.dr; do zig build run -- "$f"; done
```

Dependencies
Describe updates in `codex/_startup.sh` if new tools were added or removed.

Documentation
Note which documentation files were updated and ensure `docs/changelog.md` reflects the changes.

Checklist
- [ ] `zig build` succeeds
- [ ] All test files under `tests/` compile using `zig build run`
- [ ] Documentation updated in `docs/`
- [ ] `codex/_startup.sh` updated if dependencies changed

Additional Notes
Add any other relevant information, such as limitations or future work.
