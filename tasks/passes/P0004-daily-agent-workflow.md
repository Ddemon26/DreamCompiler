# P0004 - Daily-agent instructions and documentation flow

- Date: 2026-11-03
- Task: explicit user request for daily scheduled development and commit/push breadcrumbs
- Parent checkpoint: `6fe29c4663e2a09be403f4f937783c5fc6a6d846`
- Commit lookup: exact `Pass-ID: P0004` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Add concise root instructions, scoped test-tool instructions, a current-state handoff,
an actionable backlog, numbered pass records and a template, an accurate source map,
and a reusable daily prompt. Each scheduled invocation completes one bounded pass.
The delivery gate requires a commit, a push to the approved public branch, and a
matching remote SHA before another pass. Unfinished user work and diverged history
are preserved. The prompt includes explicit scoped publication authorization and
bootstrap instructions for a fresh automation-created worktree.

CI is triggered for integration-branch pushes as well as main and pull requests.
The scheduling guide links the official OpenAI instructions and explains project
availability, worktree isolation, permissions, and a manual first-run check. The user
will select the daily time and paste the prompt; this pass does not create a schedule.

## Validation

Documentation-only pass: 31 local links in 14 documents validated; required prompt
handoff paths and public-branch publication/blocker guards checked; root/scoped
instruction sizes and pass IDs checked. Diff review and `git diff --check` pass.

Compiler evidence is reused from P0003: 110 Dream passes, 19 skips, no failures/errors;
16 module cases within four Go groups; five runner checks; Linux cross-build. Those
compiler checks are not claimed as a new run for this documentation pass.

## Limits and handoff

Scheduled jobs still require a working local toolchain, GitHub credentials and allowed
network/filesystem operations. No prompt can override a denied approval. A scheduled
run stops on a blocker and reports recovery instructions. Native Linux checks depend
on CI; cross-build success is not native runtime evidence.

Next ready task: T001, a bounded module-signature correctness pass from the backlog.
