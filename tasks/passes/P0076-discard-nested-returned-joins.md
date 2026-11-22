# P0076 - Discard the final join from two returned appends

- Date: 2026-11-22 UTC
- Task: T011
- Parent checkpoint: `33603311d37daf7d5c24e85b4e4ef1a20e55673b` (P0075)
- Commit lookup: exact `Pass-ID: P0076` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A standalone call to a factory returning two proven appends discarded the final
allocation's pointer. The allocation stayed live until global shutdown.

The expression-statement emitter now applies the two-append factory proof to
the discarded call. The existing release consumes the original final join
immediately after normal return. The call and its arguments execute once.

The [Dream fixture](../../tests/basics/memory/owned_call_discard.dr),
[Go regression](../../codex/go/owned_call_discard_test.go), and
[checked harness](../../codex/c/owned_call_discard_harness.c) exercise
ordinary and imported factory definitions. The
[allocation audit](../../codex/python/owned_result_audit.py) isolates retention
before shutdown. See the
[discard rule](../../docs/development/string-ownership.md#standalone-calls-that-discard-two-returned-appends-p0076).

## Validation

- The active schedule uses the existing local checkout. Fetch and
  `git ls-remote` matched the clean, attached P0075 parent. Its native Windows
  and Ubuntu jobs passed at that SHA in
  [P0075 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44969995144).
- The new focused lifetime regression failed on P0075 at the assertion that
  the original final join was released before the probe returned, in both
  modes (17.379s, `-count=1`). The isolated eight-call audit made 32 releases
  and left eight final joins live per mode (9.145s).
- Zig version was **0.17.1**. The native Windows build and the Linux cross-build
  (`zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`)
  passed **15/15** steps each.
- The first post-change focused run passed the assignment and discard groups.
  The initializer group's cross-consumer assertion still expected a bare
  `nested();` statement in both modes. Updated that assertion to require the
  exact release. The focused rerun of assignment, discard and initializer
  passed (57.333s, `-count=1`). Windows Go quick passed all **43** groups
  (394.374s, `-count=1`).
- The checked discard harness verifies 10,000 calls per mode for each of the
  zero-, one- and two-append cases. Each two-append call makes three string
  copies, two joins and five releases, with no live allocation after the call.
  It releases the inner join during the outer append and the original final
  join at reference count one before returning to the probe. The fixture
  verifies one primitive argument increment, and the generated C checks the
  release and expression boundaries.
- `zig build test` discovered 187 entries on Windows. **169 passed**,
  **18 skipped**, and zero failed or errored (162.29s). Report
  `test_results_20261122_000310_windows.json` confirms exact fixture output and
  the same skip paths and reasons as P0075.
- `python codex/python/owned_result_audit.py` passed **60** probes (215.615s).
  Eight standalone calls made 40 releases and left zero final joins live per
  mode. All 58 existing measurements matched P0075.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed **5/5** (0.121s).
- Go formatting, diff review, `git diff --check`, 14 added local links and anchors,
  and all 76 pass index entries checked. Logs are in `build/P0076-*.log`.

## Limits and handoff

Discard cleanup applies to complete synchronous call statements with up to two
proven returned appends. Loop headers, conditional expressions, nested arguments,
and longer chains follow ordinary emission. Direct Console output and Console
wrappers still use the one-append proof. General argument ownership, async and
exception unwinding remain open.

Verify P0076 publication and native CI, then qualify release after direct
Console output of the two-append factory result. The
`returned_join_append_return` audit still retains eight final joins after
eight calls per mode. Cover both Console writers.
