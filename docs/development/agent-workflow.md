# Daily agent workflow

The durable project contract is [AGENTS.md](../../AGENTS.md). The current handoff is
[tasks/STATE.md](../../tasks/STATE.md); the queue is [tasks/TODO.md](../../tasks/TODO.md).
Conversation history and scheduled-task memory are helpful context, but the repository
and verified git history carry the handoff between runs.

## Set up the daily schedule

Create a daily, project-scoped scheduled task for DreamCompiler in the desktop app.
Choose the existing local project directory `C:/Users/damon/cApps/DreamCompiler`,
with HEAD attached to `codex/compiler-stabilization`, and a daily time in
`America/Vancouver`. Do not select worktree mode or create another checkout. Use the
complete prompt in [daily-progress.md](../../codex/prompts/daily-progress.md).
Keep one scheduled writer for this integration branch and avoid overlapping manual
and scheduled passes.

The project must be available, and project-scoped local schedules require the machine
and desktop app to be running. Scheduled runs use the configured permissions; a prompt
cannot grant filesystem or network access. Before enabling the schedule, run the prompt
manually and verify GitHub authentication, Zig 0.17.1, re2c, Python, Go, and the approved
branch push. Do not weaken sandbox settings to work around a failed permission check.

These setup facts follow the official [scheduled-task documentation](https://learn.chatgpt.com/docs/automations?surface=app).
Root and nested instructions follow [AGENTS.md discovery](https://learn.chatgpt.com/docs/agent-configuration/agents-md).

## 1. Establish a safe starting point

Inspect the worktree and origin without printing credentials. The approved destination
is the public `Ddemon26/DreamCompiler` repository and `codex/compiler-stabilization`.
Run only in `C:/Users/damon/cApps/DreamCompiler` on that attached branch. If the path
or branch differs, or HEAD is detached, report the mismatch and stop. Do not create
worktrees, temporary clones, or detached checkouts to proceed. Keep one writer;
scheduled and manual runs must not edit the project concurrently.
If the checkout has pre-existing edits, preserve them. Resume only a clearly documented,
authorized unfinished pass; otherwise report the ownership blocker without modifying
or publishing those changes.

```sh
git status --short
git rev-parse --show-toplevel
git branch --show-current
git fetch origin
git log -5 --oneline
```

Fetch must succeed before selecting new work. Compare `HEAD` with
`origin/codex/compiler-stabilization`:

- If they match, proceed.
- If clean `HEAD` is an ancestor of the remote tip, a local checkout of the integration
  branch may fast-forward with `git merge --ff-only origin/codex/compiler-stabilization`.
- If local commits are unpublished, finish only their clearly documented, authorized
  pass delivery first. Do not publish unrelated user commits or start a new feature.
- If history diverged, an earlier run is still active, or ownership of local changes
  is unclear, preserve the state and report the blocker. Do not reset, rebase, or force.

Before allocating a new pass ID, inspect `git log --all` and `git worktree list --porcelain`
for an unpublished earlier agent pass, and review the preceding scheduled run when its
status is available. Recover a blocked earlier run in the primary checkout or report
it for attention. If legacy changes remain in another worktree, report them for safe
recovery before proceeding; do not discard them or repeat the task in another worktree.
Do not inspect or alter unrelated user work beyond what is needed to identify ownership.

Useful read-only ancestry checks:

```sh
git merge-base --is-ancestor HEAD origin/codex/compiler-stabilization
git merge-base --is-ancestor origin/codex/compiler-stabilization HEAD
```

## 2. Select one bounded objective

Read the root instructions and current handoff. Check available CI for the previous
pass. Repair a regression first; otherwise select the first ready backlog task and
its acceptance criteria. If a task needs several passes, choose a complete, testable
slice and leave the larger item open. Do not expand into an unrelated feature.

Allocate the next unused pass ID from the [pass index](../../tasks/passes/README.md).
Use [the template](../../tasks/passes/TEMPLATE.md) for the record. If baseline testing
reveals a failure, distinguish a reproducible compiler failure from an environment
failure and record the evidence. Do not label skipped fixtures as tested features.

## 3. Implement, verify, and document

Change code and regressions together. Use the focused checks for the selected task,
then the required checks in `AGENTS.md`. Inspect the actual output and return codes.
Do not run compiler tests concurrently when they share `build/bin/dream.c` and `dream`.
Cross-builds may use a separate prefix; native runtime claims require native execution.

Update the pass record, state, backlog, and any changed feature documentation. The
current test snapshot lives in `tasks/MostRecentTestResults.md`; raw local reports in
`test_results/` are ignored. Store concise counts and commands in the pass record so
the evidence remains readable after ignored artifacts disappear.

For a documentation-only change, validate local links, referenced commands and paths,
instruction consistency, and `git diff --check`. Do not rerun the compiler suite just
to restate the preceding pass's result.

## 4. Commit the pass

Review the diff, stage only the intended files, and create a commit with this shape:

```text
[dreamc][P####] Describe the resulting behavior

Problem: concrete trigger or previous limitation.
Result: what now works, with important scope limits.
Validation: commands and measured outcomes.
Handoff: the next bounded task or remaining blocker.

Pass-ID: P####
Base-Commit: <verified parent SHA>
```

Write multiline commit messages to an ignored file and pass `git commit --file` that
file. Do not embed shell substitutions in the message. Existing early checkpoints
P0001/P0002 predate trailers; their actual commit SHAs are preserved in the pass index.

Records store a parent SHA and stable pass ID instead of a self-referential commit SHA.
Find the resulting commit with:

```sh
git log --all --grep='^Pass-ID: P0004$' --format='%h %s' -1
```

## 5. Push and verify before another pass

```sh
git push origin HEAD:refs/heads/codex/compiler-stabilization
git rev-parse HEAD
git ls-remote origin refs/heads/codex/compiler-stabilization
```

The local SHA must equal the remote branch SHA. On rejection or a changed remote tip,
preserve the commit and stop; do not use a force push. If a permission system denies
publication, report the exact repository, branch, payload, and reason. Authentication
or explicit approval must be restored through the normal controls.

After verified publication, report the pass ID, commit link, tests, unverified limits,
and next task. Inspect CI when available and distinguish pending checks from passing
checks. The next daily run repairs a failed CI baseline before taking new feature work.
End the invocation after its one pass, or after reporting a blocker; do not start a
second pass merely because time remains.
