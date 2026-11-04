# Ordered compiler backlog

Read `STATE.md` first. Current measurements live in `MostRecentTestResults.md`;
implemented language boundaries live in `../codex/FEATURES.md`. Do one bounded pass,
commit and push it, and verify the remote SHA before starting another.

A baseline regression or failed CI takes priority over this queue. Select the first
ready item; if it spans several passes, finish one useful, tested slice and leave the
remaining acceptance criteria open. Update this queue with evidence when work lands.

## T001 - Primitive function return checks (completed in P0005)

P0005 checks exact primitive return types and rejects missing values and void return
expressions before C emission. Methods use their own return context; nested switch/try
bodies and both conditional branches are visited. Four dependency-return cases require
the originating source line and absence of generated artifacts. Matching returns run
in Dream regressions and the module matrix. Numeric policy is documented in
`../docs/language-features/functions.md`.

Recovery validation: 114 Dream passes, 19 skips, no failures/errors; four Go quick
groups passed (23 diagnostic cases and 20 module cases included); five runner tests
passed; Linux cross-build succeeded. See [P0005](passes/P0005-primitive-return-checks.md).
Missing-return proofs, pointer/array shapes, and broader operator validation remain
separate follow-ups. Verify P0005 publication before starting T002.

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

- Prove required returns on all control-flow paths; extend pointer/array return shapes
  and general operator operand validation beyond the primitive scalar checks in P0005.
- Honor instance field declaration initializers. P0005's initial method fixture printed
  `(null)` for `string label = "label"`; constructor assignment works and is used by
  the final return regression. This existing limitation is outside T001.

- Broaden module exports beyond functions; design namespaces and type ownership first.
- Repair or retire the unsupported legacy Go/cgo suites with replacement coverage.
- Virtual dispatch, access controls, and implicit base construction.
- Async/concurrency and graphics remain experimental, outside the supported baseline.
