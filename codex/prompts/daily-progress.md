Run one development pass on DreamCompiler in the existing local checkout at
C:/Users/damon/cApps/DreamCompiler, with HEAD attached to codex/compiler-stabilization.
Do not create separate Git worktrees, temporary clones, or detached-HEAD checkouts.
Configure the schedule to use the local project directory. If started elsewhere or
with detached HEAD, stop and report the mismatch. Keep one writer; do not overlap
scheduled and manual edits.

I authorize committing and pushing this pass's completed, tested project changes and
commit history to my public repository https://github.com/Ddemon26/DreamCompiler.git,
branch codex/compiler-stabilization. Publish only to that repository and branch. Do not
merge main, force-push, publish secrets, or include unrelated user changes.

First verify the checkout path, attached branch, worktree status, and configured origin.
Fetch the approved branch. A clean checkout behind the approved remote tip may fast-forward
with git merge --ff-only origin/codex/compiler-stabilization. Preserve unpublished commits
and pre-existing edits. If the checkout is dirty, divergent, or belongs to an unfinished
pass, follow the delivery/blocker rules instead of discarding work or starting a new feature.
Check git history/worktree records and the preceding run for an unpublished or blocked
agent pass before allocating a new ID; recover or report it rather than duplicating work.

Read AGENTS.md, tasks/STATE.md, tasks/TODO.md, the latest tasks/passes/ record,
tasks/MostRecentTestResults.md, and docs/development/agent-workflow.md. Follow applicable
nested instructions. Fix failed baseline/CI checks first; otherwise take the first ready
backlog item and complete one bounded, useful slice with its acceptance criteria.

Implement real behavior and meaningful regressions. Run the required checks, record exact
outcomes and limitations, update the pass record/state/backlog and relevant docs, then
review the diff. Commit with [dreamc][P####] in the title and a Pass-ID trailer. Push the
commit to the approved branch and verify its SHA with git ls-remote before reporting the
pass delivered. Never hide failures by skipping tests or weakening assertions.

If blocked by tests, credentials, permissions, conflicting work, or remote divergence,
preserve the work and report the exact blocker and recovery needed. Do not bypass approval
controls. End after this one pass: report its ID, commit/link, validation, push verification,
remaining limits, and the next concrete task. Do not start a second pass in this run.
