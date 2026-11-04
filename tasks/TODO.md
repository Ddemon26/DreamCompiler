# Ordered compiler backlog

Read `STATE.md` first. Current measurements live in `MostRecentTestResults.md`;
implemented language boundaries live in `../codex/FEATURES.md`. Do one bounded pass,
commit and push it, and verify the remote SHA before starting another.

A baseline regression or failed CI takes priority over this queue. Select the first
ready item; if it spans several passes, finish one useful, tested slice and leave the
remaining acceptance criteria open. Update this queue with evidence when work lands.

## T001 — Primitive function return checks (ready)

Module calls now check primitive argument signatures, but function return expressions
are not comprehensively checked by semantic analysis. Establish and enforce return-value
compatibility without relying on C compiler errors or accidentally rejecting valid code.

Acceptance:
- Matching primitive return values continue to compile and execute.
- Incompatible return expressions and invalid bare returns receive Dream diagnostics
  before C emission, including when they occur in an imported module.
- Diagnostics identify the originating dependency source; add focused Go/Dream cases.
- Document any numeric-conversion policy used; preserve the existing annotated baseline.
- Keep broader control-flow proofs for missing returns separate if they need another pass.

## T002 — Member-call and static/instance diagnostics (ready)

Close cases where an unresolved receiver or invalid static/instance access reaches the
C compiler instead of a Dream diagnostic. Cover ordinary code and module entry classes.

Acceptance: a focused negative/positive matrix, no generated artifacts for rejected
programs, useful source locations, and preserved existing inheritance/module behavior.

## T003 — Close a skipped-regression gap (ready)

Choose one of the 19 currently skipped entry files, verify its intended behavior against
the implemented language, and make it an honest executable or rejection regression.
Fix the real compiler defect if needed. Do not annotate an unsupported program as passing,
move it out merely to improve counts, or change expected behavior without an explanation.

Acceptance: one newly meaningful checked behavior, a recorded before/after result, and
an accurate explanation of the remaining skips. Supporting module files remain fixtures.

## T004 — Connect optimization to emitted code (needs a bounded design)

The driver optimizes and discards a CFG, then emits C from the AST; SSA construction is
disabled. Plan and implement a small end-to-end optimization slice with preserved behavior.
Do not describe optimizer source files alone as working generated-code optimizations.

Acceptance for the first slice: behavior-equivalence tests at supported optimization
levels, evidence the transformed representation affects emitted code, and explicit
fallback/error behavior for unsupported lowering. Split larger SSA/backend work into
subsequent passes with durable handoffs.

## Later work

- Broaden module exports beyond functions; design namespaces and type ownership first.
- Repair or retire the unsupported legacy Go/cgo suites with replacement coverage.
- Virtual dispatch, access controls, and implicit base construction.
- Async/concurrency and graphics remain experimental, outside the supported baseline.
