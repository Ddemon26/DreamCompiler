# DreamCompiler agent instructions

## Purpose and first reads

Progress the experimental Dream compiler in small, verified passes. Dream sources
are C11; Zig 0.17.1 builds the tools and links generated C. Do not infer completed
features from old documentation or a passing example alone.

At the start of a pass, read these files in order:

1. `tasks/STATE.md` — current checkpoint and handoff.
2. `tasks/TODO.md` — ordered work with acceptance criteria.
3. The latest record under `tasks/passes/` and `tasks/MostRecentTestResults.md`.
4. `docs/development/agent-workflow.md` for the delivery procedure.

Use `docs/development/source-map.md` to find implementation code. More specific
instructions apply in `codex/` and `assets/jetbrains/`. Keep editor-plugin work
separate from compiler work unless the selected task requires both.

## One pass at a time

- A pass has one bounded objective, a regression or other appropriate evidence,
  documentation, a commit, and a verified push. A scheduled invocation does one pass.
- Fix a failed baseline or failed CI before taking a feature from the backlog.
- Finish a previously blocked delivery before starting another pass. Do not
  manufacture activity with formatting churn, inflated coverage, or empty commits.
- Preserve user changes. Do not stash, reset, clean, rebase, force-push, or rewrite
  commits to make a run proceed. Report an unsafe or ambiguous starting state.
- Do not start another feature while a pass is uncommitted or its push is unverified.

## Branch and publication agreement

The approved public destination is `https://github.com/Ddemon26/DreamCompiler.git`,
branch `codex/compiler-stabilization`. The user authorized publishing completed,
tested project passes there. Do not change remotes, publish elsewhere, merge into
`main`, or use force pushes without a new user instruction.

Use only the existing checkout at `C:/Users/damon/cApps/DreamCompiler`, with HEAD
attached to `codex/compiler-stabilization`, for manual and scheduled runs. Do not
create separate Git worktrees, temporary clones, or detached-HEAD checkouts. Configure
scheduled runs to use the local project directory. If a run starts elsewhere or with
detached HEAD, stop and report the checkout mismatch instead of creating another
workspace. A new explicit user instruction is required to change this policy.

Keep one writer in this checkout; do not overlap manual and scheduled edits. Preserve
unfinished changes in place and recover their documented pass before starting new work.
Verify the checkout path, attached branch, and clean worktree before new work, fetch
the approved branch, and follow the synchronization rules in the workflow guide.
Publish completed passes with:

```sh
git push origin HEAD:refs/heads/codex/compiler-stabilization
git ls-remote origin refs/heads/codex/compiler-stabilization
```

The remote SHA must equal the intended pass commit. A rejected push, missing
credentials, denied approval, divergence, or failed validation is a blocker. Preserve
the work and report it; never bypass the permission system or call the pass delivered.

## Implementation and verification

- Use the real parser, semantic analysis, code generator, and runtime paths. Avoid
  test-name special cases, canned compiler outputs, and silent unsupported behavior.
- Keep Linux and Windows working. Generated lexer code is not hand-edited; change
  `src/lexer/lexer.re` or `tokens.def` and let re2c regenerate it.
- Add meaningful regression coverage for behavior changes. Do not exclude a failing
  test or weaken an assertion to make the suite green. Explain genuine fixture changes.
- Inspect `git diff` and run `git diff --check` before committing.

For compiler or test-runner changes, run the relevant focused tests and:

```sh
zig version                 # must be 0.17.1
zig build test              # full annotated Dream entry suite
python -m unittest discover -s codex/python -p test_test_manager.py
```

Also run the Go quick protocol from `codex/go`:

```sh
make quick-validate         # Linux
```

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick
```

`go test ./...` includes broken legacy cgo directories and is not the supported
validation command. Go tests launching the external compiler must use `-count=1`.
On Windows, cross-build relevant C changes with
`zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`. A cross-build does not
prove Linux runtime behavior; inspect CI when available.

Documentation-only passes need link/path checks and diff review; reuse prior
compiler results with their date and pass ID, explicitly saying they were not rerun.

## Durable evidence and handoff

- Allocate the next unused `P####` ID from `tasks/passes/README.md`.
- Write `tasks/passes/P####-short-name.md` using `TEMPLATE.md`; record the parent SHA,
  objective, behavior, exact validation outcomes, limits, and the next concrete step.
- Update `tasks/STATE.md`, the pass index, and the backlog. Update test results only
  when checks actually run; update feature and user docs when behavior changes.
- Use a commit title `[dreamc][P####] <concrete result>` and a `Pass-ID: P####` trailer.
  A record cannot contain its own commit SHA; resolve it by the trailer in git history.
- Commit only this pass's files, push, and verify the remote before reporting success.
  End with the pass ID, commit/link, validation, remaining skips or limits, push result,
  and next task. If blocked, give the exact failed action and the recovery needed.
