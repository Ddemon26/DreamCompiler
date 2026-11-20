# P0068 - Release proven returned joins after Console wrapper calls

- Date: 2026-11-20 UTC
- Task: T011
- Parent checkpoint: `b31e96c1662a166bcbbbfb082fc19e1b8d3a9765` (P0067)
- Commit lookup: exact `Pass-ID: P0068` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete `view(joined());` statement kept the original join live until
shutdown. The emitter now accepts the same proven returned joins as direct
Console statements when the resolved wrapper body consists of a single
`Console.Write` or `Console.WriteLine` of its sole scalar string parameter.
The caller evaluates the factory once, passes the original joined pointer to
the synchronous wrapper, and releases it on return. This also works with
lowered imported functions.

The [Go regression](../../codex/go/owned_call_argument_test.go) and
[checked harness](../../codex/c/owned_call_argument_harness.c) verify original
allocation identity and release order in both modes. The annotated Dream
fixture checks output and the factory argument's side effects.
See the [ownership rule](../../docs/development/string-ownership.md#returned-joins-passed-to-console-wrappers-p0068).

## Validation

- Fetch and `git ls-remote` matched clean, attached P0067. Both native CI
  jobs passed at the exact parent SHA. See the
  [P0067 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44653585390).
- `python codex/python/owned_result_audit.py` passed all 42 probes at baseline
  (61.556s) and after the change (55.628s). In each mode eight wrapper calls
  went from 16 releases and eight live joins to 24 releases and zero live
  joins. The other 40 measurements matched.
- Zig 0.17.1 native build and Linux cross-build
  (`zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`)
  passed 15/15 steps each. Native Linux behavior is checked separately by CI.
- Four focused Go groups passed (17.580s, `-count=1`). Per mode and writer,
  10,000 wrapper calls passed original-pointer and reference-count checks,
  once-only argument evaluation and an empty registry after each probe.
  Direct Console checks ran alongside the new wrapper checks. Outputs matched
  in the checked harness and the normally linked program. Windows also
  cross-linked the harness for Linux.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  all five tests (0.075s).
- `zig build test` discovered 182 entries on Windows. 164 passed, 18 skipped,
  and zero failed or errored (64.58s). Report
  `test_results_20261120_000920_windows.json` confirms exact output for the
  updated fixture and the same 18 unannotated skip paths as P0067.
- Windows Go quick (`powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`) passed all 43 groups
  (144.413s, `-count=1`).
- Reviewed the diff and verified 14 added local links and anchors, the unique
  pass index entry, and `git diff --check`. Logs are in `build/P0068-*.log`.

## Limits and handoff

Conditional arguments, calls in larger expressions, unproven factories and
wrappers with forwarding or additional statements keep general expression
emission. Async and exception unwinding remain open. Verify P0068 publication
and both native CI jobs, then measure retention of the factory's original join
in a complete `text += joined();` statement to an owned local string in both
compilation modes.
