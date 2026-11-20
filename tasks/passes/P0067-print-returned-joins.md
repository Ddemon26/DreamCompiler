# P0067 - Release proven returned joins after direct Console output

- Date: 2026-11-20 UTC
- Task: T011
- Parent checkpoint: `2472f30b3ba8afaf1bc191fd88a1ec7634e2797b` (P0066)
- Commit lookup: exact `Pass-ID: P0067` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete `Console.WriteLine(joined());` statement retained the original
join until shutdown. Direct `Console.Write` and `Console.WriteLine` statements
now use the same returned-join proof as standalone discards. The statement
evaluates the factory and its arguments once, passes the original pointer to
the synchronous writer, and releases the join after output.

The [Go regression](../../codex/go/owned_call_argument_test.go) and
[checked harness](../../codex/c/owned_call_argument_harness.c) exercise
ordinary and imported factories. The existing annotated Dream fixture adds
output from both writers and the final argument-evaluation count. See the
[ownership rules](../../docs/development/string-ownership.md#direct-console-output-of-returned-joins-p0067).

## Validation

- Fetch and `git ls-remote` matched clean, attached P0066. Both native CI
  jobs passed at the exact parent SHA. See the
  [P0066 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44645544287).
- The P0066 compiler passed the expanded baseline ownership audit (40 probes,
  129.145s). The eight isolated Console calls made 16 releases and left eight
  joins live in each mode.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  all five tests (0.112s).
- Zig 0.17.1 native build passed 15/15 steps.
- The first focused Go run failed after 72.372s. Both new lifetime checks passed.
  The append-return fixture still expected 12 live allocations, and the
  initializer matrix expected a bare Console call. The regressions now require
  the two additional releases and the new Console cleanup block. A new
  expression control assumed direct string ternary output, which the emitter
  does not support. The final control checks a direct concatenation and a
  conditional argument to a wrapper.
- The final focused Go run passed five groups (92.346s, `-count=1`).
  Each writer passed 10,000 original-pointer checks per mode. The harness
  verified one argument increment, two operand copies, one join and three
  releases per call. The original join was the only live allocation during the
  write and was released at reference count one. Checked and normally linked
  output matched, and the harness cross-linked for Linux. The append-return
  fixture now measures 31 releases and 10 survivors per mode, up from 29
  releases and down from 12 survivors.
- `python codex/python/owned_result_audit.py` passed all 40 probes (102.430s).
  Eight direct Console calls made 24 releases and left zero surviving joins
  in each mode. The other 38 measurements matched the baseline.
- `zig build test` discovered 182 entries on Windows. 164 passed, 18 skipped,
  and zero failed or errored (114.94s). Report
  `test_results_20261119_181621_windows.json` confirms exact output for the
  updated fixture and the same 18 unannotated skip paths as P0066.
- Windows Go quick (`powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`) passed all 43 groups
  (496.996s, `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is checked separately by CI.
- Reviewed the diff and checked nine added local links and anchors, the unique
  pass index entry, and `git diff --check`. Logs are in ignored
  `build/P0067-*.log`.

## Limits and handoff

The proof is limited to a complete direct Console statement whose entire operand
is a proven call. Conditional expressions, unproven join factories and wrapper
calls retain their existing behavior. Async and exception unwinding remain open.
Verify publication and both native CI jobs, then measure the lifetime of the
original join in a complete `view(joined());` call to a proven Console wrapper
in ordinary and module-entry modes.
