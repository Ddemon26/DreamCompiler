Run one development pass on DreamCompiler, using this project's isolated Git worktree.

I authorize committing and pushing this pass's completed, tested project changes and
commit history to my public repository https://github.com/Ddemon26/DreamCompiler.git,
branch codex/compiler-stabilization. Publish only to that repository and branch. Do not
merge main, force-push, publish secrets, or include unrelated user changes.

First inspect the worktree and configured origin. Fetch the approved branch. In a fresh,
clean automation worktree, if HEAD equals or is an ancestor of the approved remote tip,
you may switch detached to origin/codex/compiler-stabilization so the latest instructions
and handoff are available. Preserve unpublished commits and pre-existing edits. If the
checkout is dirty, divergent, or belongs to an unfinished pass, follow the delivery/blocker
rules instead of discarding work or starting a new feature.
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
