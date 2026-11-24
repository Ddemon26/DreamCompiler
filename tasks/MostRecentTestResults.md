# Most Recent Test Results - DreamCompiler

**Date:** November 24, 2026 UTC

**Pass:** P0083 - release the final join from two returned appends in a directly returned append

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0082 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0082 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45256716669).
- `zig build test` discovered **192** entries. **174 passed**, **18 skipped**,
  with zero failures or errors. Report `test_results_20261123_190115_windows.json`
  confirms exact fixture output and matching statuses and skip reasons for all
  191 earlier entries.
- Four focused Go groups passed (54.760s, `-count=1`). The new regression
  checked 25 nonnull releases and five still-registered allocations for each
  mode before shutdown. The caller read the concatenation after return, and
  the replacement argument's side effect ran once.
- Windows Go quick passed all **43** groups (146.671s Go suite time, `-count=1`).
- All **76** allocation audit probes passed (73.957s). Eight calls in each
  mode made 56 releases and left eight returned allocations live. P0082 made
  48 releases and left sixteen joins live per mode. All 74 earlier probes
  matched.
- Native Windows and Linux cross-builds passed **15/15** steps each. Native
  Linux execution is tracked in CI.
- Python runner unit tests passed **5/5** (0.061s).

See the [P0083 record](passes/P0083-return-appends-with-nested-joins.md) for scope and limits.

---

## Historical P0082 checkpoint

**Date:** November 23, 2026 UTC

**Pass:** P0082 - release the final join from two returned appends before a Console wrapper prints an append

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0081 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0081 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45194744940).
- `zig build test` discovered **191** entries. **173 passed**, **18 skipped**,
  with zero failures or errors. Report `test_results_20261123_130638_windows.json`
  confirms exact fixture output and the same statuses and skip reasons for all
  190 earlier entries.
- Four focused Go groups passed (44.732s, `-count=1`). Each checked binary
  made 22 nonnull releases for the new regression and retained only the join
  from the extra-statement wrapper before shutdown. Both proven wrappers
  printed the installed string, which remained readable afterward.
- Windows Go quick passed all **43** groups (144.218s Go suite time, `-count=1`).
- All **74** allocation audit probes passed (74.874s). Eight calls through each
  wrapper made 56 nonnull releases and left zero factory joins live in each
  mode. P0081 made 48 releases and left eight live. All 70 earlier probes
  matched.
- Native Windows and Linux cross-builds passed **15/15** steps each. Native
  Linux execution is tracked in CI.
- Python runner unit tests passed **5/5** (0.048s).

See the [P0082 record](passes/P0082-wrap-appends-with-nested-joins.md) for scope and limits.

---

## Historical P0081 checkpoint

**Date:** November 23, 2026 UTC

**Pass:** P0081 - release the final join from two returned appends before printing an append

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0080 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0080 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45142890890).
- `zig build test` discovered **190** entries. **172 passed**, **18 skipped**,
  with zero failures or errors. Report `test_results_20261123_072451_windows.json`
  confirms exact fixture output and the same statuses and skip reasons for all
  189 earlier entries.
- Four focused Go groups passed (38.045s, `-count=1`). Each checked binary
  made 16 nonnull releases for the new regression and left zero allocations
  live before shutdown. Both writers printed the installed string, which
  remained readable as the destination afterward.
- Windows Go quick passed all **43** groups (129.001s, `-count=1`).
- All **70** allocation audit probes passed (62.467s). Eight calls to each
  writer made 56 nonnull releases and left zero joins live in each mode.
  P0080 made 48 releases and left eight live. All 66 earlier probes matched.
- Native Windows and Linux cross-builds passed **15/15** steps each. Native
  Linux execution is tracked in CI.
- Python runner unit tests passed **5/5** (0.031s).

See the [P0081 record](passes/P0081-print-appends-with-nested-joins.md) for scope and limits.

---

## Historical P0080 checkpoint

**Date:** November 23, 2026 UTC

**Pass:** P0080 - release the final join from two returned appends in a snapshot initializer

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0079 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0079 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45092901302).
- `zig build test` discovered **189** entries. **171 passed**, **18 skipped**,
  with zero failures or errors. Report `test_results_20261123_015234_windows.json`
  confirms exact fixture output and the same statuses and skip reasons for all
  188 earlier entries.
- Four focused Go groups passed (40.859s, `-count=1`). The checked ordinary and
  module-entry executables each made 90,019 nonnull releases and left zero live
  allocations before shutdown. The snapshot remained readable after destination
  replacement, and the destination remained readable after snapshot cleanup.
- Windows Go quick passed all **43** groups (166.191s, `-count=1`).
- All **66** allocation audit probes passed (80.877s). Eight isolated
  initializers made 72 releases and left zero factory joins live in each mode.
  P0079 made 64 releases and left eight live. All 64 previous probes matched.
- Native Windows and Linux cross-builds passed **15/15** steps each. Native
  Linux execution is tracked in CI.
- Python runner unit tests passed **5/5** (0.045s).

See the [P0080 record](passes/P0080-initialize-from-nested-join-appends.md) for scope and limits.

---

## Historical P0079 checkpoint

**Date:** November 23, 2026 UTC

**Pass:** P0079 - release the final join from two returned appends after a complete append statement

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0078 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0078 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45058950909).
- `zig build test` discovered **188** entries. **170 passed**, **18 skipped**,
  with zero failures or errors (57.43s). Report
  `test_results_20261122_180443_windows.json` confirms exact output for the new
  fixture and the same statuses and skip reasons for all 187 earlier entries.
- Four focused Go groups passed (40.277s, `-count=1`). The checked ordinary and
  module-entry executables each made 70,009 releases, including the calls that
  exercised replacement during argument evaluation and borrowed self-append.
- Windows Go quick passed all **43** groups (136.797s, `-count=1`).
- All **64** allocation audit probes passed (71.254s). Eight isolated appends
  made 56 releases and left zero joins live in each mode, compared with 48
  releases and eight live joins on P0078. All 62 previous probes matched.
- Native Windows and Linux cross-builds passed **15/15** steps each. Native
  Linux execution is tracked in CI.
- Python runner unit tests passed **5/5** (0.030s).

See the [P0079 record](passes/P0079-append-nested-returned-joins.md) for scope and limits.

---

## Historical P0078 checkpoint

**Date:** November 22, 2026 UTC

**Pass:** P0078 - release the final join from two returned appends after a Console wrapper returns

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0077 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0077 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45026250607).
- `zig build test` discovered **187** entries. **169 passed**, **18 skipped**,
  with zero failures or errors (53.94s). Report
  `test_results_20261122_120313_windows.json` confirms exact wrapper output
  and the same statuses and unannotated skip paths as P0077.
- Both focused Go groups passed (9.501s, `-count=1`). Each writer read the
  original final join in 10,000 checked calls through its wrapper per mode.
- Windows Go quick passed all **43** groups (128.533s, `-count=1`).
- All **62** allocation audit probes passed (64.829s). Eight wrapper calls
  made 40 releases and left zero final joins live in each mode. P0077 made
  32 releases and left eight live. The other 60 measurements matched P0077.
- Native Windows and Linux cross-builds passed **15/15** steps each. Native
  Linux execution is checked separately by CI.
- Python runner unit tests passed **5/5** (0.046s).

See the [P0078 record](passes/P0078-wrap-nested-returned-joins.md) for scope and limits.

---

## Historical P0077 checkpoint

**Date:** November 22, 2026 UTC

**Pass:** P0077 - release the final join from two returned appends after direct Console output

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0076 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0076 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44997427544).
- `zig build test` discovered **187** entries. **169 passed**, **18 skipped**,
  with zero failures or errors (66.04s). Report
  `test_results_20261122_060729_windows.json` confirms exact output for both
  writers and the same entry statuses, skip paths and reasons as P0076.
- Both focused Go groups passed (11.509s, `-count=1`). Each writer read
  the original final join before caller release in 10,000 checked calls per mode.
- Windows Go quick passed all **43** groups (155.605s, `-count=1`).
- All **62** allocation audit probes passed (77.149s). Eight direct writes
  left zero final joins live per mode, down from eight on P0076. The 58
  remaining P0076 measurements matched. Two new wrapper probes each left
  eight final joins live.
- Native Windows and Linux cross-builds passed **15/15** steps each. Native
  Linux execution is checked separately by CI.
- Python runner unit tests passed **5/5** (0.067s).

See the [P0077 record](passes/P0077-print-nested-returned-joins.md) for scope and limits.

---

## Historical P0076 checkpoint

**Date:** November 22, 2026 UTC

**Pass:** P0076 - discard the final join from two returned appends

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0075 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0075 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44969995144).
- `zig build test` discovered **187** entries. **169 passed**, **18 skipped**,
  with zero failures or errors (162.29s). Report `test_results_20261122_000310_windows.json`
  confirms the discard fixture's exact output and the same skip paths and reasons
  as P0075.
- Windows Go quick passed all **43** groups (394.374s, `-count=1`). The
  focused assignment, discard, and initializer rerun passed (57.333s). The checked discard
  harness verified 10,000 two-append calls per mode with once-only argument
  evaluation and release of the original final pointer at reference count one.
- All **60** allocation audit probes passed (215.615s). Eight standalone calls
  left zero final joins live in each mode, down from eight on P0075. All 58
  earlier measurements matched P0075.
- Native Windows and Linux cross-builds passed **15/15** steps each. Native
  Linux execution is checked separately by CI.
- Python runner unit tests passed **5/5** (0.121s).

See the [P0076 record](passes/P0076-discard-nested-returned-joins.md) for scope and limits.

---

## Historical P0075 checkpoint

**Date:** November 22, 2026 UTC

**Pass:** P0075 - adopt the final join from two returned appends in a scalar assignment

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0074 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0074 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44939209746).
- `zig build test` discovered **187** entries. **169 passed**, **18 skipped**,
  with zero failures or errors (56.26s). Report `test_results_20261121_180340_windows.json`
  shows identical skip paths and reasons to P0074.
- The focused Go assignment and initializer regressions passed (13.047s,
  `-count=1`). In each mode, 10,000 scopes checked the original pointers and
  release order for two assignments per scope, including RHS argument replacement.
- All **58** ownership audit probes passed (63.806s). Eight assignments left
  zero live joins in each mode, down from eight on P0074. The other 56
  measurements matched P0074.
- Windows Go quick passed all **43** groups (133.490s, `-count=1`).
- Windows native and Linux cross-builds passed **15/15** steps each.
  Native Linux execution is checked separately by CI.
- Python runner unit tests passed **5/5** (0.032s).

See the [P0075 record](passes/P0075-assign-nested-returned-joins.md) for scope and limits.

---

## Historical P0074 checkpoint

**Date:** November 21, 2026 UTC

**Pass:** P0074 - adopt the final join from two returned appends

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0073 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0073 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44907693639).
- `zig build test` discovered **187** entries. **169 passed**, **18 skipped**,
  with zero failures or errors (50.34s). Report `test_results_20261121_120323_windows.json`
  shows the same skip paths and reasons as P0073.
- The focused Go initializer and append regressions passed (29.915s, `-count=1`).
  Each of 10,000 nested initializer calls per mode released the original final
  join at reference count one and emptied the allocation registry. The existing
  append-return fixture made 49 releases per mode and left two unproven final
  results live before shutdown.
- All **56** ownership audit probes passed (56.143s). The eight calls went from
  eight live joins to zero in each mode. The other 54 measurements matched P0073.
- Windows Go quick passed all **43** groups (123.073s, `-count=1`).
- Windows native and Linux cross-builds passed **15/15** steps each.
  Native Linux execution is checked separately by CI.
- Python runner unit tests passed **5/5** (0.043s).

See the [P0074 record](passes/P0074-adopt-nested-returned-joins.md) for scope and limits.

---

## Historical P0073 checkpoint

**Date:** November 21, 2026 UTC

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0072 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0072 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44879397034).
- `zig build test` discovered **187** entries. **169 passed**, **18 skipped**,
  with zero failures or errors (53.81s). Report
  `test_results_20261121_060912_windows.json` confirms exact output for the
  new fixture and the same skip paths and reasons as P0072.
- The focused Go regression passed in ordinary and module-entry modes
  (2.930s, `-count=1`). Both checked and normally linked programs produced
  the expected output. Each mode made **49** checked releases and had **10**
  final results registered before shutdown.
- All **54** ownership audit probes passed (58.677s). In each new probe,
  eight calls went from 24 releases and 16 live allocations to 32 releases
  and eight final results. The other 52 measurements matched P0072.
- Windows Go quick passed all **43** groups (135.255s, `-count=1`).
- Windows native and Linux cross-builds passed **15/15** steps each.
  Native Linux execution is checked separately by CI.
- Python runner unit tests passed **5/5** (0.047s).

See the [P0073 record](passes/P0073-return-appends-with-returned-joins.md) for scope and limits.

---

## Historical P0072 checkpoint

**Date:** November 21, 2026 UTC

**Pass:** P0072 - release proven returned joins in appends passed to Console wrappers

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0071 CI passed on Windows and Ubuntu at the verified parent SHA.
  See the [P0071 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44848733343).
- `zig build test` discovered **186** entries. **168 passed**, **18 skipped**,
  with zero failures or errors (143.88s). Report
  `test_results_20261121_000614_windows.json` confirms exact output for the
  new fixture and the same skip paths and reasons as P0071.
- Four focused Go groups passed (104.574s, `-count=1`). Each mode checked
  10,000 repeated wrapper calls, 40,009 nonnull releases at reference count one,
  and zero live allocations before shutdown. The destination remained readable
  after the wrapper returned.
- All **52** ownership audit probes passed (138.854s). Eight qualified calls
  per mode went from 32 releases and eight live joins to 40 releases and
  zero live joins. The other 50 measurements matched the parent.
- Windows Go quick passed all **43** groups (349.757s, `-count=1`).
- Windows native and Linux cross-builds passed **15/15** steps each.
  Native Linux execution is checked separately by CI.
- Python runner unit tests passed **5/5** (0.153s).

See the [P0072 record](passes/P0072-wrap-appends-with-returned-joins.md) for scope and limits.

---

## Historical P0071 checkpoint

**Date:** November 21, 2026 UTC

**Pass:** P0071 - release proven returned joins in appends printed by Console

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0070 CI passed on Windows and Linux at the verified parent SHA.
  See the [P0070 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44799863088).
- `zig build test` discovered **185** entries. **167 passed**, **18 skipped**,
  with zero failures or errors (51.96s). Report
  `test_results_20261120_180917_windows.json` confirms the new fixture's exact
  output and the same skip paths and reasons as P0070.
- Four focused Go groups passed (34.786s, `-count=1`). The new regression
  verified both Console writers, 10,000 repeated writes, 40,009 checked releases,
  and zero live allocations before shutdown per mode. The updated destination
  remained readable after printing.
- All **48** ownership audit probes passed (54.564s). Eight printed appends per
  mode went from 32 releases and eight live joins to 40 releases and zero live
  joins. The other 46 measurements matched baseline.
- Windows Go quick passed all **43** groups (129.200s, `-count=1`).
- Windows native and Linux cross-builds passed **15/15** steps each.
  Native Linux execution is checked separately by CI.
- Python runner unit tests passed **5/5** (0.063s).

See the [P0071 record](passes/P0071-print-appends-with-returned-joins.md) for scope and limits.

---

## Historical P0070 checkpoint

**Date:** November 20, 2026 UTC

**Pass:** P0070 - release proven returned joins in append initializers

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0069 CI passed on Windows and Linux at the verified parent SHA.
  See the [P0069 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44741352440).
- `zig build test` discovered **184** entries. **166 passed**, **18 skipped**,
  with zero failures or errors (49.98s). Report
  `test_results_20261120_120858_windows.json` confirms exact output for the
  new initializer fixture and the same skip paths and reasons as P0069.
- Four focused Go groups passed (28.555s, `-count=1`). In each mode the new
  regression verified 10,000 repeated initializers, independent destination
  and snapshot lifetimes, 70,015 checked releases, and zero live allocations
  before shutdown.
- All **46** ownership audit probes passed (46.217s). Eight initializers per
  mode went from 48 releases and eight live joins to 56 releases and zero
  live joins. The other 44 measurements matched baseline.
- Windows Go quick passed all **43** groups (115.464s, `-count=1`).
- Windows native and Linux cross-builds passed **15/15** steps each.
  Native Linux execution is checked separately by CI.
- Python runner unit tests passed **5/5** (0.044s).

See the [P0070 record](passes/P0070-initialize-from-returned-join-appends.md) for scope and limits.

---

## Historical P0069 checkpoint

**Date:** November 20, 2026 UTC

**Pass:** P0069 - release proven returned joins after complete append statements

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0068 CI passed on Windows and Linux at the verified parent SHA.
  See the [P0068 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44693285083).
- `zig build test` discovered **183** entries. **165 passed**, **18 skipped**,
  with zero failures or errors (53.23s). Report
  `test_results_20261120_060253_windows.json` confirms exact output for the
  new append fixture and the same skip paths and reasons as P0068.
- Four focused Go groups passed (28.877s, `-count=1`). In each mode the new
  regression verified 10,000 repeated appends, post-argument destination
  reads, 50,007 checked releases and zero live allocations before shutdown.
- All **44** ownership audit probes passed (48.313s). Eight appends per mode
  went from 32 releases and eight live joins to 40 releases and zero live joins.
  The other 42 measurements matched baseline.
- Windows Go quick passed all **43** groups (127.214s, `-count=1`).
- Windows native and Linux cross-builds passed **15/15** steps each.
  Native Linux execution is checked separately by CI.
- Python runner unit tests passed **5/5** (0.038s).

See the [P0069 record](passes/P0069-append-returned-joins.md) for scope and limits.

---

## Historical P0068 checkpoint

**Date:** November 20, 2026 UTC

**Pass:** P0068 - release proven returned joins after Console wrapper calls

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0067 CI passed on Windows and Linux at the verified parent SHA.
  See the [P0067 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44653585390).
- `zig build test` discovered **182** entries. **164 passed**, **18 skipped**,
  with zero failures or errors (64.58s). Report
  `test_results_20261120_000920_windows.json` confirms exact output for the
  wrapper fixture and the same skip paths as P0067.
- Four focused Go groups passed (17.580s, `-count=1`). Each writer passed
  10,000 original-pointer checks through the wrapper per mode, with once-only
  argument evaluation and no retained joins.
- The ownership audit passed **42** probes (55.628s). Eight isolated wrapper
  calls per mode went from 16 releases and eight survivors to 24 releases
  and zero survivors. The other 40 measurements matched baseline.
- Python runner unit tests passed **5/5** (0.075s).
- Windows Go quick passed all **43** groups (144.413s, `-count=1`).
- Windows native and Linux cross-builds passed **15/15** steps each.
  Native Linux execution is checked separately by CI.

See the [P0068 record](passes/P0068-wrap-returned-joins.md) for scope and limits.

---

## Historical P0067 checkpoint

**Date:** November 20, 2026 UTC

**Pass:** P0067 - release proven returned joins after direct Console output

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0066 CI passed on Windows and Linux at the verified parent SHA.
  See the [P0066 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44645544287).
- `zig build test` discovered **182** entries. **164 passed**, **18 skipped**,
  with zero failures or errors (114.94s). Report
  `test_results_20261119_181621_windows.json` confirms exact output for the
  updated Console fixture and the same skip paths as P0066.
- Five focused Go groups passed (92.346s, `-count=1`). Each Console writer
  passed 10,000 original-pointer checks per mode with once-only argument
  evaluation and no retained joins.
- The ownership audit passed **40** probes (102.430s). Eight isolated Console
  calls per mode moved from 16 releases and eight survivors to 24 releases
  and zero survivors. The other 38 measurements matched the baseline.
- Python runner unit tests passed **5/5** (0.112s).
- Windows Go quick passed all **43** groups (496.996s, `-count=1`).
- Windows native and Linux cross-builds passed **15/15** steps each.
  Native Linux execution is checked separately by CI.

See the [P0067 record](passes/P0067-print-returned-joins.md) for scope and limits.

---

## Historical P0066 checkpoint

**Date:** November 20, 2026 UTC

**Pass:** P0066 - release proven returned joins after standalone calls

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0065 CI passed on Windows and Linux at the verified parent SHA.
  See the [P0065 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44595800169).
- `zig build test` discovered **182** entries. **164 passed**, **18 skipped**,
  with zero failures or errors (102.63s). Report
  `test_results_20261119_171916_windows.json` confirms the discard fixture
  passed and the skip paths match P0065.
- Focused Go discard, initializer, assignment, and argument groups passed
  (26.331s, `-count=1`). Ordinary and imported module calls each passed
  **10,000** checked discards, verifying once-only argument evaluation,
  release of the original join at reference count one, and an empty registry.
- The ownership audit passed **38** probes (64.188s). In each mode, eight
  isolated discards went from 16 releases and eight surviving joins to
  24 releases and zero survivors. The other 36 measurements matched baseline.
- Python runner unit tests passed **5/5** (0.065s).
- Windows Go quick passed all **43** groups (207.748s, `-count=1`).
- Windows native and Linux cross-builds passed **15/15** steps each.
  Native Linux execution is verified separately by CI.

See the [P0066 record](passes/P0066-discard-returned-joins.md) for scope and limits.

---

## Historical P0065 checkpoint

**Date:** November 19, 2026 UTC

**Pass:** P0065 - adopt proven returned joins in scalar string assignments

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0064 CI passed on Windows and Linux at the verified parent SHA.
  See the [P0064 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44530574682).
- `zig build test` discovered **182** entries. **164 passed**, **18 skipped**,
  with zero failures or errors (41.04s). Report
  `test_results_20261119_120914_windows.json`.
- Focused Go assignment, initializer and append regressions passed (19.207s).
  Ordinary and imported module calls each ran **10,000** checked scopes with
  two join assignments per scope. Each scope verified the exact allocation
  bases and release order, reference count one, a peak of four live
  allocations and an empty registry.
- The ownership audit passed **36** probes (31.375s). Eight isolated assignments
  in each mode made 32 releases, with survivors down from eight to zero.
- Python runner unit tests passed **5/5** (0.036s).
- Windows Go quick protocol passed all **43** groups (108.450s, `-count=1`).
- Windows native build and Linux cross-build passed **15/15** steps each.
  Native Linux runtime verification comes from CI.

See the [P0065 record](passes/P0065-assign-returned-joins.md) for scope and limits.

---

## Historical P0064 checkpoint

**Date:** November 19, 2026 UTC

**Pass:** P0064 - adopt proven returned joins in scalar string initializers

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0063 [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44477695219)
  passed on Windows and Linux at the verified parent SHA.
- `zig build test` discovered **182** entries. **164 passed**, **18 skipped**,
  with zero failures or errors (60.27s). Report
  `test_results_20261119_060528_windows.json`.
- Focused Go initializer and return-append regressions passed (7.550s).
  Ordinary and imported module calls each ran **10,000** checked declarations.
  Each declaration released the original join at reference count one and left
  the registry empty. Both checked and normally linked programs matched exact
  output. The P0063 return-append regression still measured 29 releases and
  12 survivors per mode.
- The ownership audit passed **34** probes (33.860s). Eight isolated calls in
  each mode had 24 releases, with survivors down from eight to zero.
- Python runner unit tests passed **5/5** (0.068s).
- Windows Go quick protocol passed all **43** groups (141.473s, `-count=1`).
- Windows native build and Linux cross-build passed **15/15** steps each.
  Native Linux runtime verification comes from CI.

See the [P0064 record](passes/P0064-adopt-returned-joins.md) for scope and limits.

---

## Historical P0063 checkpoint

**Date:** November 19, 2026 UTC

**Pass:** P0063 - release proven factory operands in direct append returns

**Platform/toolchain:** Windows with Zig 0.17.1.

- P0062 [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44434733409)
  passed on Windows and Linux at the verified parent SHA.
- `zig build test` discovered **182** entries. **164 passed**, **18 skipped**,
  with zero failures or errors (58.44s). Report
  `test_results_20261119_000727_windows.json`.
- Focused Go return-append regression passed in ordinary and module-entry modes
  (4.457s). Each mode verified exact output, **29** nonnull releases, and
  **12** live returned allocations before shutdown.
- The ownership audit passed **34 probes** (41.896s). The isolated eight-call
  return went from **16 releases and 16 survivors** to **24 releases and eight
  survivors** in each mode. The caller could read the join throughout.
- Python test-runner unit tests passed **5/5** (0.059s).
- Windows Go quick protocol passed **43** groups (164.436s, `-count=1`).
- Windows native build and Linux cross-build passed **15/15** build steps each.

See the [P0063 record](passes/P0063-owned-append-return.md) for the remaining
caller ownership limits. Native Linux runtime verification comes from CI.

---

## Historical P0062 checkpoint

**Date:** November 19, 2026 UTC

**Pass:** P0062 - restore CI Python setup on Ubuntu 26.04

**Platform/toolchain:** Windows, Zig 0.17.1, Python 3.13.17.

- P0061 [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44376422082)
  failed during Python 3.8 setup on Ubuntu 26.04. The Windows job was canceled.
- Updated workflow passed `actionlint` v1.7.11 with identical 43-group Go
  selectors in CI, Make, and PowerShell.
- `zig build test`: **181 discovered, 163 passed, 18 skipped, zero failures
  or errors** (58.35s). Report: `test_results_20261118_180634_windows.json`.
- Python test-runner unit tests: **5 passed** (0.047s).
- Windows Go quick protocol: **43 groups passed** (127.753s, `-count=1`).
- Linux cross-build: **15/15** build steps succeeded.

See the [P0062 record](passes/P0062-ci-python-setup.md). Native Linux validation
of P0062 comes from CI.

---

## Historical P0061 checkpoint

**Date:** November 18, 2026 UTC

**Pass:** P0061 - release proven factory operands in appends passed to Console wrappers

**Platform/toolchain:** Windows, Zig 0.17.1.

- P0060 exact-parent native Linux and Windows [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44323725290)
  passed.
- `zig build test`: **181 discovered, 163 passed, 18 skipped, zero failures
  or errors** (51.75s). Report: `test_results_20261118_120053_windows.json`.
- Focused append, initializer, argument, and assignment Go groups: **four
  passed** (23.043s). The new regression checks 10,000 wrapper appends per mode,
  20,008 nonnull releases, zero allocations live before shutdown, and
  successful destination reuse after the wrapper returns.
- `python codex/python/owned_result_audit.py`: **32 probes passed** (34.977s).
  The isolated wrapper append moved from **16 releases and eight survivors**
  in P0060 to **24 releases and zero survivors** in each mode. The
  two-statement wrapper control retained one original in each mode.
- Python test-runner unit tests: **5 passed** (0.053s).
- Windows Go quick protocol: **43 groups passed** (116.625s, `-count=1`).
- Native build and Linux cross-build: **15/15** steps succeeded each. Native
  Linux runtime validation for P0061 is tracked in CI.

See the [P0061 record](passes/P0061-owned-append-wrapper.md) for details and
the remaining ownership cases.

---

## Historical P0060 checkpoint

**Date:** November 18, 2026 UTC

**Pass:** P0060 - release proven factory operands in printed string appends

**Platform/toolchain:** Windows, Zig 0.17.1.

- P0059 exact-parent native Linux and Windows [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44315369022)
  passed.
- `zig build test`: **180 discovered, 162 passed, 18 skipped, zero failures
  or errors** (53.92s). Report: `test_results_20261118_072040_windows.json`.
- Focused append, initializer, argument, and assignment Go groups: **four
  passed** (21.628s). The new regression checks 10,000 printed appends per
  mode, 20,008 nonnull releases, zero allocations live before shutdown, and
  successful destination reuse after printing.
- `python codex/python/owned_result_audit.py`: **28 probes passed** (32.914s).
  The isolated printed append moved from **16 releases and eight survivors**
  in P0059 to **24 releases and zero survivors** in each mode.
- Python test-runner unit tests: **5 passed** (0.047s).
- Windows Go quick protocol: **43 groups passed** (120.827s, `-count=1`).
- Native build and Linux cross-build: **15/15** steps succeeded each. Native
  Linux runtime validation for P0060 is tracked in CI.

See the [P0060 record](passes/P0060-owned-append-console.md) for details and
the remaining ownership cases.

---

## Historical P0059 checkpoint

**Date:** November 18, 2026 UTC

**Pass:** P0059 - release proven factory operands in string append initializers

**Platform/toolchain:** Windows, Zig 0.17.1.

- P0058 exact-parent native Linux and Windows [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44310991032)
  passed.
- `zig build test`: **179 discovered, 161 passed, 18 skipped, zero failures
  or errors** (55.29s). Report: `test_results_20261118_063302_windows.json`.
- Focused append, initializer, argument, and assignment Go groups: **four
  passed** (20.189s). The new regression checks 10,000 append initializers
  per mode, 50,013 nonnull releases, zero allocations live before shutdown,
  and independent destination/snapshot storage.
- `python codex/python/owned_result_audit.py`: **26 probes passed** (31.859s).
  The isolated initializer moved from **32 releases and eight survivors**
  in P0058 to **40 releases and zero survivors** in both modes.
- Python test-runner unit tests: **5 passed** (0.036s).
- Windows Go quick protocol: **43 groups passed** (128.677s, `-count=1`).
- Native build and Linux cross-build: **15/15** steps succeeded each. Native
  Linux runtime validation for P0059 is tracked in CI.

See the [P0059 record](passes/P0059-owned-append-initializer.md) for details and
the remaining ownership cases.

---

## Historical P0058 checkpoint

**Date:** November 18, 2026 UTC

**Pass:** P0058 - release proven factory operands after owned string append

**Platform/toolchain:** Windows, Zig 0.17.1.

- P0057 exact-parent native Linux and Windows [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44298800009)
  passed.
- `zig build test`: **178 discovered, 160 passed, 18 skipped, zero failures
  or errors** (59.72s). Report: `test_results_20261118_060712_windows.json`.
- Focused append, initializer, argument, and assignment Go groups: **four
  passed** (17.051s). The new subtest checks 10,000 append repetitions per
  mode, 30,012 nonnull releases per mode, zero survivors before shutdown, exact
  generated C, and exact checked and normally linked output.
- `python codex/python/owned_result_audit.py`: **24 probes passed** (28.084s).
  The isolated append moved from **16 releases and eight survivors** at P0057
  to **24 releases and zero survivors** in each mode. The aggregate went from
  **six releases and one survivor** to **seven releases and zero survivors**.
- Python test-runner unit tests: **5 passed** (0.044s).
- Windows Go quick protocol: **43 groups passed** (131.950s, `-count=1`).
- Native build and Linux cross-build: **15/15** steps succeeded each. Native
  Linux runtime validation for P0058 is tracked in CI.

See the [P0058 record](passes/P0058-owned-call-append.md) for the remaining
ownership cases and validation evidence.

---

## Historical P0057 checkpoint

**Date:** November 18, 2026 UTC

**Pass:** P0057 - release proven factory results after direct Console output

**Platform/toolchain:** Windows, Zig 0.17.1.

- P0056 exact-parent native Linux and Windows [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44259186333)
  passed.
- `zig build test`: **177 discovered, 159 passed, 18 skipped, zero failures
  or errors** (57.21s). Report: `test_results_20261118_044750_windows.json`.
- Focused Go argument, discard and indexed Console groups: **three passed**
  (7.009s). The checked identity harness exercised **10,000 direct calls per
  writer and 10,000 wrapper calls per mode**. It observed the original
  allocation at reference count one and zero survivors after each release.
  Generated-C, exact-output and Linux harness cross-link checks passed.
- `python codex/python/owned_result_audit.py`: **22 probes passed** (26.942s).
  Each direct writer went from **0 releases/8 live** at P0056 to **8/0** in
  both modes. The aggregate went from **5/2** to **6/1**.
- Python test-runner unit tests: **5 passed** (0.052s).
- Windows Go quick protocol: **43 groups passed** on rerun (124.747s,
  `-count=1`). The initial run exposed a Console emission expectation in the
  initializer fixture, which now checks the complete release sequence.
- Native build and Linux cross-build: **15/15** steps succeeded each. The
  local Linux check was a cross-build.

See the [P0057 record](passes/P0057-direct-console-ownership.md) for the rules
and the remaining ownership cases.

---

## Historical P0056 checkpoint

**Date:** November 18, 2026 UTC

**Pass:** P0056 - release proven factory arguments after Console wrappers

**Platform/toolchain:** Windows, Zig 0.17.1.

- P0055 exact-parent native Linux and Windows [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44213186405)
  passed.
- `zig build test`: **177 discovered, 159 passed, 18 skipped, zero failures
  or errors** (58.96s). Report: `test_results_20261118_002529_windows.json`.
- Focused Go wrapper and parser recovery groups: **both passed** (4.608s).
  The native checked harness verifies **10,000 calls per mode**. The factory
  original stays readable through Console output, releases at reference count
  one and never leaves more than one live allocation. Generated-C checks,
  normal linking, Linux harness cross-links and exact output passed.
- `python codex/python/owned_result_audit.py`: **18 probes passed** (20.912s).
  Per mode, the isolated wrapper now has **8 releases/0 live** before
  shutdown and the aggregate has **5/2**. The static field and sibling
  controls still each have **0/1**.
- Python test-runner unit tests: **5 passed** (0.055s).
- Windows Go quick protocol: **43 groups passed** (119.340s, `-count=1`).
- Native build and Linux cross-build: **15/15** steps succeeded each. The
  local Linux check was a cross-build.

See the [P0056 record](passes/P0056-owned-call-argument.md) for the rules and
the remaining ownership cases.

---

## Historical P0055 checkpoint

**Date:** November 18, 2026 UTC

**Pass:** P0055 - audit proven factory results passed as arguments

**Platform/toolchain:** Windows, Zig 0.17.1.

- P0054 exact-parent [CI 44193733124](https://github.com/Ddemon26/DreamCompiler/actions/runs/44193733124)
  passed natively on Linux and Windows.
- `zig build --summary all`: **15/15 succeeded**.
- `python codex/python/owned_result_audit.py`: **18 probes passed** (23.658s).
  Per mode, eight isolated `view(owned())` calls left **zero nonnull releases
  and eight live originals** before shutdown. The static-store and sibling
  controls each left **zero releases and one live original**. Existing
  initializer, assignment, discard, and aggregate counts were unchanged.
  Generated-C and exact stdout checks, native UBSan execution, normal linking
  and Windows-to-Linux harness cross-links passed in both modes.
- Reused P0054's November 17 full Dream suite (**158 passed, 18 skipped,
  zero failures or errors**), **42 Go quick groups**, **five Python runner
  tests**, and **15/15 Linux cross-build steps**. Those suites and the full
  cross-build were not rerun in this audit.

See the [argument contract](../docs/development/owned-call-argument.md) and
[P0055 record](passes/P0055-owned-call-argument-audit.md). Compiler emission is
unchanged. Release after the selected wrapper returns remains to be implemented.

---

## Historical P0054 checkpoint

**Date:** November 17, 2026 UTC

**Pass:** P0054 - release discarded proven factory results

**Platform/toolchain:** Windows, Zig 0.17.1.

- P0053 exact-parent native Linux and Windows [CI 44155266595](https://github.com/Ddemon26/DreamCompiler/actions/runs/44155266595)
  passed.
- Baseline `zig build test`: **175 discovered, 157 passed, 18 skipped, zero failures
  or errors** (66.82s, `test_results_20261117_154217_windows.json`).
- Before the fix, eight isolated discarded calls added eight live originals
  and zero nonnull releases in each mode. The focused Go test failed the expected
  generated-C release gates in both modes (0.643s).
- Focused Go ownership groups: **90 subtests passed** (11.602s, `-count=1`).
  The new group verifies 10,000 original-pointer releases per mode at reference
  count one and peak live count one. No survivor remains after any call. Checked
  and normally linked execution produce identical output for the scoped fixture.
- Ownership audit: **12 probes passed** (13.901s). Per mode, isolated discard
  reached **8 releases/0 live** and the aggregate other-consumer case **4/3**.
  Prior assignment and initializer cases retain their measured counts.
- Final `zig build test`: **176 discovered, 158 passed, 18 skipped, zero failures
  or errors** (57.49s, `test_results_20261117_154619_windows.json`).
- Python test-runner unit tests: **5 passed** (0.042s).
- Windows Go quick protocol: **42 groups passed** (122.767s, `-count=1`).
- Linux cross-build: **15/15 steps succeeded**. Native P0054 Linux runtime
  verification is tracked by CI.

See [P0054](passes/P0054-owned-call-discard.md) for exact commands and scope.
Append, Console output and argument consumption still retain results in the
aggregate case. The next audit starts with `view(owned())`.

---

## Historical P0052 checkpoint

**Date:** November 17, 2026 UTC

**Pass:** P0052 - adopt complete-statement owned factory assignments

**Platform/toolchain:** Windows; Zig 0.17.1.

- Parent P0051 native Linux/Windows [CI 44036460739](https://github.com/Ddemon26/DreamCompiler/actions/runs/44036460739)
  passed. Clean approved checkout/attached branch/origin equals fetched parent.
- Baseline `zig build test`: **174 discovered/156 passed/18 skipped/zero failures
  or errors**, **48.11s**, `test_results/test_results_20261117_060416_windows.json`.
- Pre-fix ten-probe audit: **10.564s**, both isolated assignment modes retain
  **three originals/four nonnull releases**. Focused pre-fix test fails both
  adoption C gates, **0.793s**; excluded context controls pass.
- Final two focused groups: **86 subtests pass, 7.771s** (12 assignment, 74
  initializer). Existing 28 definition controls in each mode additionally check
  assignment fallback. Each mode executes 100,000 replacements across 10,000 calls,
  original-pointer release order/refcount one, peak live two, empty after each call.
  Separate return identity and main **19 copies/19 releases** pass with exact output,
  native UBSan, Linux cross-link and normal linking. Prior initializer evidence is
  unchanged. Only eligible assignment C expectations migrate.
- Final ten-probe audit: **10.197s**, isolated assignment **4 releases/0 live**,
  other consumers **3/4** (previous 3/5); owned initializers **6/0**, borrowed **7/0**,
  mixed **4/1** unchanged, per mode. No other consumer assertion changes.
- Final `zig build test`: **175 discovered/157 passed/18 skipped/zero failures or
  errors**, **44.56s**, `test_results/test_results_20261117_060815_windows.json`.
- Python runner unittest command: **five pass, 0.032s**.
- Supported Windows Go quick protocol: **41 groups pass, 96.283s**, `-count=1`.
- Linux cross-build: **15/15 steps pass**. Cross-building/linking does not prove
  native Linux P0052 execution; inspect exact-commit CI after publication.
- Documentation/path, UTF-8, selectors, 52-pass index, BOM, gofmt, Python syntax and
  diff review pass. Checked 254 local Markdown paths and 19 UTF-8 pass files.
  Logs: ignored `build/P0052-*.log`. See [P0052](passes/P0052-owned-call-assignment.md)
  for environment/initial expectation/helper corrections and limits.

The eighteen skips persist. General assignment expressions, remaining consumers,
mixed/method results, nested owned arguments, aliases/containers/destructors/fields,
pointer/class transfer and async/exception unwinding remain separate. Top-level
storage retains program lifetime. Next verify publication/native CI, then audit
complete discarded factory-call statements before implementing another consumer.

---

# Historical P0050 checkpoint

# Most Recent Test Results - DreamCompiler

**Date:** November 17, 2026 UTC

**Pass:** P0050 - adopt proven owned free-function string initializers

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4.

- Approved clean checkout/attached branch/origin equals published P0049
  `f31b174eb5bad57334c915b769d3c7f63cb3ff66`. Exact-parent
  [native Linux/Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43993419794)
  passed build, Python, quick and full suite. One registered worktree; available
  chats show no competing active project writer. Schedule uses local approved cwd.
- Baseline `zig build test`: **173 discovered, 155 passed, 18 skipped, zero
  failures/errors**, **61.36s**, `test_results/test_results_20261116_192137_windows.json`.
- Corrected pre-fix both-mode regression fails the exact adoption C assertions,
  **0.702s** package time. The C gate stops before runtime. Initial new fixture
  corrections and existing grammar/semantic limits are explained in
  [P0050](passes/P0050-owned-call-initializer.md); no prior assertion is weakened.
- Final focused `go test -count=1 -v -run '^TestCompiler_OwnedCallInitializer$' .`:
  **74 subtests pass**, **6.829s** package time. Each mode checks 10,000 original
  pointer releases/refcount one, peak live count one, zero survivors after each
  call, a separate string-transfer probe, native UBSan, Linux harness cross-link
  and checked/directly linked exact output. Main fixture: **16 copies/15 scoped
  releases** ordinary, **17/16** module (one borrowed import copy). Only the exact
  original top-level global remains at shutdown with refcount one, then none.
- Both modes cover actual forward/imported definitions and same-spelled borrowed
  versus owned factories, snapshots/replacement, absent/typed-null/escaped/empty/
  const/grouped calls, scopes/headers/loops/jumps/returns/collisions. Fifty-six
  emission-only definition controls, four consumer/primitive-positive controls
  and twelve artifact-free unresolved/shadow/binding/null/async rejections pass.
- Supported Windows Go quick protocol: **all 40 groups pass, 151.212s**;
  external compiler tests use `-count=1`. The final focused rerun follows a
  test-only imported-definition discrimination addition; compiler and all other
  group inputs were unchanged. P0037-P0048 fixture/count/refcount assertions
  remain unchanged and pass; no genuine prior fixture migration was required.
- Final `zig build test`: **174 discovered, 156 passed, 18 skipped, zero failures/
  errors**, **80.83s**, `test_results/test_results_20261116_193319_windows.json`
  (local date in result filenames).
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five pass, 0.081s**.
- `python codex/python/owned_result_audit.py`: **eight probes pass, 12.184s**.
  Per mode owned initializers now **6 releases/0 live** (P0049: 6/5); borrowed
  **7/0**, mixed **4/1** and other consumers **3/5** remain unchanged. Checked
  release operands/refcounts, exact C/output, native UBSan and Linux cross-links
  pass. Only eligible C/live counts supersede the measured P0049 checkpoint.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **15/15 steps pass**. Cross-build/cross-linking does not prove native Linux
  P0050 execution; inspect exact-commit CI after publication.
- Final review: **243 local Markdown paths**, **18 UTF-8 pass files**, 50-pass
  index continuity, equal quick/CI selectors matched to all 40 passing groups,
  PowerShell BOM, gofmt, Python syntax and diff checks pass. Existing lifetime
  fixtures/harness/assertions are unchanged. Logs: ignored `build/P0050-*.log`.

The eighteen skips and previous grammar/operator/shape limits persist. This is
only direct declaration adoption from the two-statement proven factory. Top-level
storage keeps program lifetime. General/mixed/method results, nested owned argument
temporaries, other consumers, aliases/containers/destructors/fields, pointer/class
transfer and async/exception unwinding remain separate. Next verify publication/CI,
then audit plain assignment from the same factories into already-owned scalar slots.

---

# Historical P0049 checkpoint - DreamCompiler

**Date:** November 17, 2026 UTC

**Pass:** P0049 - owned string call-result audit and adoption contract

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4.

- Approved clean checkout/attached branch/origin matches P0048
  `7f356d9ab1e2145f0f10fe31142f3db34a09072b`. Its exact-head
  [native Linux/Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43935983779)
  passed build, runner, Go quick and full Dream suite. One registered worktree;
  available chats show no competing active writer. Schedule uses local approved cwd.
- `zig build test`: **173 discovered, 155 passed, 18 skipped, zero failures/errors**,
  **49.46s**, `test_results/test_results_20261116_180209_windows.json` (local date).
  Compiler sources are unchanged; this is the final compiler checkpoint as well.
- `python codex/python/owned_result_audit.py`: **eight probes pass, 9.439s**,
  ordinary/module-entry modes. Per mode, owned initializer originals: **6 releases /
  5 live**; borrowed initializer controls: **7 / 0**; mixed-provenance initializers:
  **4 / 1**; other consumers: **3 / 5**. Final survivors have original reference
  counts and shutdown empties the registry. These counts measure existing debt,
  not adoption acceptance. Exact C/output, live release operands/refcounts, native
  UBSan, Linux harness cross-link and normally linked output pass. Original allocation
  identities and peak storage are not measured by this audit.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five pass, 0.056s**.
- Supported Windows Go quick protocol: **all 39 groups pass, 101.704s**;
  external compiler tests use `-count=1`. Prior ownership assertions are unchanged.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **15/15 steps pass**. Cross-build/cross-link success is not native Linux audit
  execution. Python syntax and documentation/diff checks are in
  [P0049](passes/P0049-owned-call-result-audit.md). Logs: ignored `build/P0049-*.log`.

Restricted network/Zig access initially failed; approved normal accesses succeeded
without toolchain/cache/configuration changes. Two initial helper/validation launch
path errors occurred before tests ran and were corrected; details are in the pass.
The same eighteen skips and grammar/operator/shape limits persist. Caller adoption
is not implemented. Next verify publication/CI and implement only the
[proven free-function initializer slice](../docs/development/owned-call-result-consumption.md).

---

# Historical P0048 checkpoint - DreamCompiler

**Date:** November 16, 2026 UTC

**Pass:** P0048 - conditional read-only static string-field return cleanup

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4.

- Clean approved checkout/attached branch/origin equals published P0047
  `f06e6c84dfadfeeb93db39cd999145d126b8f16a`; exact-parent
  [native Linux/Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43874515506)
  passed build, Python, quick and full-suite steps. One registered worktree, no
  competing active writer found. Schedule uses local execution/approved cwd.
- Restricted fetch failed DNS and restricted baseline could not locate Zig's
  installation. Approved normal network/toolchain access succeeded without cache,
  configuration or permission changes.
- Baseline `zig build test`: **172 discovered, 154 passed, 18 skipped, zero failures/
  errors**, **46.94s**; `test_results/test_results_20261116_115736_windows.json`.
- Pre-fix full regression in both ordinary/module-entry modes fails the exact
  capture/cleanup C assertion, **0.535s**. No pre-fix runtime result is claimed:
  the intact assertion stops execution before the checked allocator.
- Final `go test -count=1 -v -run '^TestCompiler_ConditionalStaticReturn$' .` from
  `codex/go`: **246 subtests pass**, **9.247s**. Each lifetime mode checks **67
  nonnull releases** (one receiver, 66 strings), **35 output lines**, live release
  operands/refcounts, native UBSan, Linux harness cross-linking, directly linked
  exact output and **four original-reference owned survivors** until shutdown;
  storage is empty afterward. Independent allocation tally is in the test.
- Mixed static/instance/parameter/local/literal leaves, inherited/hidden owners,
  managed/literal/empty/typed-null backing, scope/header/loop/switch paths,
  collisions, free/static/instance readers, caller/snapshot survival and no-local
  readers are checked. Borrowed static backing is cleared before caller cleanup.
  The other 244 controls check body/leaf/shape barriers and async/exception exits.
  Former eligible static fallbacks migrate to exact positive C; existing lifetime
  assertions remain unchanged. Unsafe mutation/alias/shape sources never run.
- Initial four-group run passes prior member/conditional-member/static groups and
  both new lifetime modes, but fails one new module unselected-call expectation
  (36.343s). Corrected its existing lowered free-function symbol; no compiler,
  grammar, lifetime assertion or semantic change was needed. Native rebuild passed.
- `zig build test`: **173 discovered, 155 passed, 18 skipped, zero failures/errors**,
  **47.26s**; `test_results/test_results_20261116_120305_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five pass**, **0.036s**.
- Supported Windows Go quick protocol: **all 39 groups pass**, **93.816s**,
  conditional static **7.61s**, conditional member **6.97s**, direct member **6.62s**,
  direct static **7.90s**. External compiler tests use `-count=1`; quick/CI selectors
  equal the actual passing groups. Previous ownership/allocator checks pass.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **15/15 steps pass**. Cross-building/cross-linking does not prove native P0048
  Linux runtime behavior; inspect CI after publication.
- Documentation/encoding/BOM/index/annotation/gofmt/diff checks are recorded in
  [P0048](passes/P0048-conditional-static-return.md). Logs: ignored `build/P0048-*.log`.

The same eighteen skips and prior shape/operator/grammar limits persist. Static
value leaves require an unshadowed class qualifier, actual scalar static field/
owner and the unchanged whole-body read-only proof. Valid incoming backing remains
a caller precondition. Other static qualifiers, static pointer/array emission,
caller consumption/retained owned-result accumulation, aliases/containers/
destructors/fields, pointer/class result transfer and async/exception unwinding
remain separate. Next verify publication/native CI, then audit caller consumption
of owned string results before choosing the next bounded implementation slice.

---

# Historical P0047 checkpoint - DreamCompiler

**Date:** November 16, 2026 UTC

**Pass:** P0047 - direct read-only static string-field return cleanup

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4.

- Clean approved checkout/attached branch/origin matched published P0046
  `3e1db012b833c7060217120533d82321d05a9df9`; exact-parent
  [Linux/Windows native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43821203292)
  passed build, Python, quick and full suite. One registered worktree, no competing
  active writer found. Schedule already uses local execution/approved cwd.
- Restricted fetch/CI requests failed network access and restricted baseline could
  not locate Zig's installation. Approved normal network/toolchain access succeeded
  without changing toolchain configuration, caches or permissions.
- Baseline `zig build test`: **171 discovered, 153 passed, 18 skipped, zero failures/
  errors**, **48.75s**; `test_results/test_results_20261116_060710_windows.json`.
- Before fix, `go test -count=1 -v -run '^TestCompiler_StaticMemberReturn$' .` from
  `codex/go`: **both modes failed** the empty pre-shutdown registry assertion,
  **2.637s** (group 2.20s). Native rebuild passed; initial both-mode lifetime check
  then passed, **2.860s** (group 2.43s).
- Final static group: **252 subtests passed**, **9.015s**. Each mode checks **40
  nonnull releases** (one receiver, 39 strings), **23 output lines**, live release
  operands/refcounts, native UBSan, Linux harness cross-linking, directly linked
  exact output and empty pre/post-shutdown registries. Caller backing and independent
  snapshot survive; borrowed static backing is cleared before caller storage exits.
  Exact C checks once-only capture before reverse cleanup, inherited/hidden owners,
  free/static/instance readers, scope/header paths, collisions and no-local returns.
- Static controls comprise 208 shared controls (206 fallback, two migrated static
  positives), 30 expressions (28 fallback, two existing-instance positives), twelve
  artifact-free semantic rejections plus two lifetime modes. Earlier member groups
  passed intermediate broader runs. Initial new expectation failures omitted existing
  assignment parentheses or expected emission for shadow/index sources rejected by
  current semantics; corrected exact C/diagnostic tests preserve compiler behavior.
  No lifetime assertions or unsupported conditional controls were removed.
- `zig build test`: **172 discovered, 154 passed, 18 skipped, zero failures/errors**,
  **46.49s**; `test_results/test_results_20261116_061319_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, **0.043s**.
- Supported Windows Go quick protocol: **all 38 groups passed**, **86.012s**,
  including the static group's 252 subtests and all earlier ownership groups.
  External compiler tests use `-count=1`; existing allocator/reference/shutdown
  assertions remain intact. Both quick selectors and CI include the new group.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-building/cross-linking does not prove native P0047
  Linux runtime behavior; inspect CI after publication.
- Final documentation/diff checks are recorded in
  [P0047](passes/P0047-static-member-return.md). Logs: ignored `build/P0047-*.log`.

The same eighteen skips and prior shape/operator limits persist. Direct static
results require an unshadowed class type, actual scalar static declaration/owner
and the unchanged whole-body read-only proof. Valid incoming backing remains a
caller precondition. Conditional static leaves, other receivers, static pointer/
array declaration emission, caller consumption/retained owned-result accumulation,
class-parameter syntax, aliases/containers/destructors/fields, pointer/class result
transfer and async/exception unwinding remain separate. Next verify publication/
native CI, then qualify conditional static value leaves under the same proof.

---

# Historical P0045 checkpoint - DreamCompiler

**Date:** November 16, 2026 UTC

**Pass:** P0045 - conditional read-only instance string-field return cleanup

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- Approved clean checkout/attached branch/origin matched published P0044
  `106d95cd0db5a727e8a7f0a577670f0e81f91b93`. Its
  [native Linux/Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43793738851)
  passed build, runner, quick and full-suite steps. Schedule already uses local
  execution/approved cwd; one registered worktree, no competing active writer found.
- Restricted fetch failed DNS and restricted baseline could not locate the Zig
  installation. Approved normal network/toolchain access succeeded without
  changing toolchain configuration, caches or permissions.
- Baseline `zig build test`: **170 discovered, 152 passed, 18 skipped, zero failures/
  errors**, 50.92s; `test_results/test_results_20261115_212858_windows.json`.
- Before fix, `go test -count=1 -v -run '^TestCompiler_ConditionalMemberReturn$' .`
  from `codex/go`: **both modes failed**, **50 live allocations instead of four**,
  2.616s (group 2.17s). An earlier test setup failed a global forbidden-release
  assertion because the caller reused the parameter name; renamed the caller
  binding before lifetime reproduction. No cleanup assertion was removed.
- `zig build`: **passed**, exit 0. Initial lifetime group: **both modes passed**,
  3.184s (group 2.74s), initially 57 releases/25 outputs. Final coverage adds two
  inherited two-field calls. A broader five-group run passed the four earlier
  return groups but failed the new index control's C expectation (27.625s);
  corrected its missing existing parentheses without changing compiler behavior.
- Final focused new group: **passed**, 8.627s (group 8.18s). Each mode checks **59
  nonnull releases** (one receiver, 58 strings), **27 output lines**, **four selected
  owned results** with original references before shutdown and an empty registry
  afterward. Native UBSan/live release checks, Linux harness cross-linking and
  directly linked exact output pass. Exact C checks one capture before reverse
  cleanup, selected-local comparisons, inherited casts, collisions and no-local
  returns; no member result copy/retain or receiver release. **196 shared emission
  controls**, **14 expression rejections** and **two nested-else positives** pass.
- `zig build test`: **171 discovered, 153 passed, 18 skipped, zero failures/errors**,
  48.45s; `test_results/test_results_20261115_213338_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.044s.
- Supported Windows Go quick protocol: **all 37 groups passed**, 82.677s; new group
  **6.86s**, prior direct member group **7.09s**. External compiler tests use
  `-count=1`; existing allocator/reference-count/shutdown assertions stay intact.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-linking does not prove native P0045
  Linux runtime behavior; inspect CI after publication.
- **207 local Markdown paths**, UTF-8 in all 17 pass files, PowerShell BOM,
  37-group quick/CI selector and actual-pass equality, 27 annotated outputs,
  P0001-P0045 index continuity, gofmt, full diff review and `git diff --check`
  passed. Logs: ignored `build/P0045-*.log`.

Same eighteen skips and prior shape/operator limits. Conditional member leaves
require the unchanged whole-body read-only proof. Static/local/struct/nested/base
receivers, explicit class parameters, caller consumption, retained owned-result
accumulation, broader aliases/containers/destructors/fields and exception/async
cleanup remain separate. String-index return typing, custom-type for headers and
direct class null initializers remain limited. Next verify publication/native CI,
then audit static scalar string-field result lifetime and mutation/alias barriers.

---

# Historical P0044 checkpoint - DreamCompiler

**Date:** November 16, 2026 UTC

**Pass:** P0044 - read-only direct instance string-field return cleanup

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- Clean approved checkout/branch/origin matched published P0043
  `001c5223cc6d9e054b66f8da5f0b821f3731c007`. Its
  [native Linux/Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43784740261)
  passed build, runner, quick and full-suite steps before work. Schedule already
  uses local execution and the approved cwd; no competing active writer found.
- Restricted fetch failed DNS and restricted baseline could not locate Zig's
  installation. Approved normal network/toolchain access succeeded without
  changing toolchains, caches or permissions.
- Baseline `zig build test`: **169 discovered, 151 passed, 18 skipped, zero failures/
  errors**, 54.00s; `test_results/test_results_20261115_192459_windows.json`.
- Before fix, `go test -count=1 -v -run '^TestCompiler_BorrowedMemberReturn$' .`
  from `codex/go`: **both modes failed** at the empty-before-shutdown registry
  assertion, 2.703s (group 2.24s), after fixing test package/unsupported parameter
  scaffolding. `zig build` passed; initial both-mode lifetime check then passed
  (2.889s, group 2.44s), with 36 releases before two more positive paths were added.
- The new fallback matrix initially used self-type declarations, receiver shadowing
  and async methods rejected by current parsing/semantics. Replaced these with valid
  prior-type alias/constructor and free-async/static-field controls; retained grammar
  limitations. The broader five-group check passed prior groups and failed only the
  invalid async-method fixture (20.723s). No compiler grammar/semantics were changed.
- Final focused new group: **passed**, 8.002s, both modes plus **196 emission-only
  controls**. Each mode checks **38 nonnull releases** (one receiver, 37 strings),
  **21 output lines**, empty pre/post-shutdown registries, native UBSan/live release/
  refcount checks, Linux harness cross-linking and directly linked exact output.
  Exact C requires one capture before reverse cleanup with inherited owner cast,
  no receiver release or result retain/copy, and no-local direct returns. Caller
  receiver/backing and independent snapshot survive subsequent replacement.
- `zig build test`: **170 discovered, 152 passed, 18 skipped, zero failures/errors**,
  52.40s; `test_results/test_results_20261115_193452_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.057s.
- Supported Windows Go quick protocol: **all 36 groups passed**, 77.947s; new group **8.57s**. Prior
  owned/borrowed return allocator/refcount/shutdown assertions remain intact;
  external compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-linking does not prove native P0044
  Linux runtime behavior; inspect CI after publication.
- 201 local Markdown paths, UTF-8/BOM, 36-group quick/CI selector and actual-pass
  equality, 21 annotated outputs, P0001-P0044 index continuity, gofmt, full diff
  review and `git diff --check` passed. Precommit fetch and `git ls-remote` still
  matched the parent. Logs are ignored `build/P0044-*.log`.

Same eighteen skips and prior shape/operator limits. Only direct scalar nonstatic
`this.field` string results under the whole-body read-only proof gain cleanup.
Explicit class parameters and async instance methods remain rejected by grammar;
receiver-name shadowing by semantics. Conditional member leaves, static/local/struct/
nested/base receivers, caller consumption, retained owned results, broader aliases/
containers/destructors/fields and exception/async cleanup stay separate. String-index
return typing, custom-type for headers and direct class null initializers remain
limited. Next verify publication/native CI, then qualify conditional member leaves
under the same write/call/alias barriers.

---

# Historical P0042 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC

**Pass:** P0042 - conditional borrowed string-result scoped cleanup

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- Clean approved checkout/branch/origin matched published P0041
  `b57c2b2406f6c4621f8b30cb4b902eadc70d207d`. Its
  [native Linux/Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43751573004)
  passed build, runner, quick and full-suite steps before work. Schedule already
  uses local execution and the approved cwd; no competing active writer found.
- Restricted fetch failed DNS and restricted baseline could not locate Zig's
  installation. Approved normal network/toolchain access succeeded without
  changing toolchains, caches or permissions.
- Baseline `zig build test`: **168 discovered, 150 passed, 18 skipped, zero failures/
  errors**, 49.80s; `test_results/test_results_20261115_130630_windows.json`.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_ConditionalBorrowedReturn$' .`: **both modes failed**, 2.545s
  (group 2.11s), with **62 live allocations instead of five** before shutdown.
- `zig build`: **passed**, exit 0. Initial focused check: **both modes passed**,
  3.132s (group 2.64s), with 75 releases and five original-reference results.
- Strengthened focused checks initially failed the concatenation control's exact
  C expectation (11.770s); it omitted the emitter's existing parentheses around
  `dream_concat`. Corrected the expectation without changing emission or runtime
  assertions. Final new/P0041/P0040/P0039 checks: **all passed**, 12.613s; new group
  **3.72s**, including **48 rebinding cases**. The 24 direct-return controls remain.
- `zig build test`: **169 discovered, 151 passed, 18 skipped, zero failures/errors**,
  75.83s; `test_results/test_results_20261115_131203_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.040s.
- Supported Windows Go quick protocol: **all 35 groups passed**, 80.416s; new group
  **4.73s**. Each mode checks **75 nonnull releases** (19 classes, 56 strings),
  **39 output lines**, **five selected owned results** with original references
  before shutdown and an empty registry afterward. Native UBSan/live release/
  refcount checks, Linux harness cross-linking and directly linked output pass.
  Exact C requires capture before reverse cleanup and preserves unsupported and
  active exception returns. All 48 conditional and 24 direct rebinding cases pass;
  previous unused controls now check qualified cleanup with unchanged runtime
  assertions. External compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-linking does not prove native P0042
  Linux runtime behavior; inspect CI after publication.
- 181 local Markdown paths, 35-group quick/CI selector equality, PowerShell UTF-8 BOM,
  39 annotated output lines, gofmt, full diff review and `git diff --check` passed.
  Precommit fetch and `git ls-remote` still matched the parent. Logs are ignored
  `build/P0042-*.log`.

Same eighteen skips and prior shape/operator limits. Every conditional value leaf
must be a literal, known-owned scalar local or unchanged borrowed scalar parameter;
unsupported/rebound leaves preserve the entire prior return. Caller backing retains
its original lifetime without a result copy/retain; selected owned results retain
their original lifetime until shutdown. General member/index/call/compound result
lifetime, caller consumption, aliases/containers/destructors/fields and async/active
exception unwinding remain separate. See
[P0042](passes/P0042-conditional-borrowed-return.md). Next verify publication/native
CI, then audit direct borrowed member-result lifetime and mutation/call barriers.

---

# Historical P0041 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC

**Pass:** P0041 - cleanup on unchanged borrowed scalar string-parameter returns

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- Clean approved checkout/branch/origin matched published P0040
  `df5ddb965d2df77a5a0fe9c84b1191e77171973d`. Its
  [native Linux/Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43744953713)
  passed build, runner, quick and full-suite steps before work.
- Restricted fetch failed DNS and restricted baseline could not locate Zig's
  installation. Approved normal network/toolchain access succeeded without changing
  configuration, caches or permissions.
- Baseline `zig build test`: **167 discovered, 149 passed, 18 skipped, zero failures/
  errors**, 46.03s; `test_results/test_results_20261115_120617_windows.json`.
- Before fix, `go test -count=1 -v -run '^TestCompiler_BorrowedParameterReturn$' .`
  from `codex/go`: **both modes failed** at the empty-before-shutdown registry
  assertion, 2.505s (group 2.02s).
- `zig build`: **passed**, exit 0. Initial focused check: **both modes passed**,
  2.923s (group 2.47s), with 58 checked releases and an empty registry.
- Final focused new/P0037/P0038 checks: **all three groups passed both modes**,
  8.451s; new group 3.33s including **24 rebound-parameter cases**. Exact C requires
  result capture before reverse cleanup; unused earlier borrowed controls now
  require P0041 emission with unchanged runtime assertions.
- `zig build test`: **168 discovered, 150 passed, 18 skipped, zero failures/errors**,
  44.16s; `test_results/test_results_20261115_121123_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.035s.
- Supported Windows Go quick protocol: **all 34 groups passed**, 70.423s; new
  group **2.76s**, both modes with final exact C, native UBSan, checked live release
  operands/refcounts, Linux harness cross-linking and directly linked exact output.
  Each mode checks **58 nonnull releases** (20 classes, 38 strings), **17 output
  lines**, and empty pre/post-shutdown registries. Twenty-four additional cases
  preserve fallback for twelve placements of rebinding. Compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-linking does not prove native P0041
  Linux runtime behavior; inspect CI after publication.
- 176 local Markdown paths, 34-group quick/CI selector equality, PowerShell UTF-8
  BOM, gofmt, complete diff review and `git diff --check` passed. Precommit fetch
  and remote SHA still matched the parent. Logs are ignored `build/P0041-*.log`.

Same eighteen skips and prior shape/operator limits. Only direct unchanged scalar
parameters gain cleanup; the whole-body scan conservatively rejects rebinding,
same-name declarations and unknown nodes. Caller storage keeps its original
lifetime without a copy/retain. Rebound/conditional/member/index/call results,
retained owned-result accumulation, aliases/containers/destructors/fields and
async/active exception unwinding stay separate. See
[P0041](passes/P0041-borrowed-parameter-return.md). Next verify publication/native
CI, then qualify conditional result trees with unchanged borrowed parameter leaves.

---

# Historical P0040 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC

**Pass:** P0040 - conditional owned-local string-result preservation and cleanup

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- Clean approved checkout/branch/origin matched published P0039
  `d51ee248c642f44c96ae8b340af7bd89afd9b3f2`. Its
  [native Linux/Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43717895195)
  passed build, runner, quick and full-suite steps before work.
- Restricted fetch failed DNS and restricted baseline could not locate Zig's
  installed directory. Approved normal network/toolchain access succeeded without
  changing configuration, caches or permissions.
- Baseline `zig build test`: **166 discovered, 148 passed, 18 skipped, zero failures/
  errors**, 51.48s; `test_results/test_results_20261115_105815_windows.json`.
- Before fix, `go test -count=1 -v -run '^TestCompiler_ConditionalLocalReturn$' .`
  from `codex/go`: **both modes failed**, 2.777s (group 2.30s), with **57 live
  allocations instead of 12** before shutdown.
- `zig build`: **passed**, exit 0. Initial after-fix check passed live-count checks
  but found **61 releases versus 60 expected** (2.292s): the hand count omitted
  caller backing replacement. Corrected that count, then added candidate replacement
  and equal-content cases, independently totaling 64 releases and 14 live results.
- Focused new/P0039/P0037 groups: **all three passed both modes**, 8.498s; new
  group 2.79s. Final exact C assertions were subsequently checked by quick validation.
- `zig build test`: **167 discovered, 149 passed, 18 skipped, zero failures/errors**,
  49.51s; `test_results/test_results_20261115_110144_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.047s.
- Supported Windows Go quick protocol: **all 33 groups passed**, 65.331s; new
  group **2.23s**, both modes with final exact C checks, native UBSan, checked live
  release operands/reference counts, Linux harness cross-linking and linked output.
  Each requires **64 nonnull releases** (18 classes, 46 strings), **36 output lines**,
  exactly **14 selected allocations** with one reference before shutdown and an empty
  registry afterward. Earlier allocator/refcount/shutdown assertions remain intact;
  external compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-linking does not prove native P0040
  Linux runtime behavior; inspect CI after publication.
- 171 local Markdown paths resolved, 33-group quick/CI selector equality, PowerShell UTF-8 BOM,
  gofmt, complete diff review and `git diff --check` passed. Precommit fetch and
  remote SHA still matched the parent.

Same eighteen skips and prior shape/operator limits. Conditional trees require
literal or known-owned-local value leaves; unsupported leaves preserve the entire
prior return. Selected owned allocations keep their original lifetime until
shutdown; caller declarations still copy without consuming the original. General
result/borrowed-source ownership, async/active exception cleanup and aliases/
containers/destructors/fields remain separate. See
[P0040](passes/P0040-conditional-local-return.md). Next verify publication/native CI,
then audit direct borrowed string-parameter return cleanup and caller backing.

---

# Historical P0039 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC

**Pass:** P0039 - cleanup before literal-only conditional string returns

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0038 publication matched parent `ea001f013a7fff6b78b30b065d7012c8ca4d9d2c`;
  [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43698784933)
  passed Linux/Windows build, runner, quick and full-suite steps before work.
- Restricted fetch failed DNS; restricted baseline could not locate Zig's installed
  directory. Approved normal network/toolchain access succeeded without configuration,
  cache or permission changes.
- Baseline `zig build test`: **165 discovered, 147 passed, 18 skipped, zero failures/
  errors**, 45.65s; `test_results/test_results_20261115_060439_windows.json`.
- Before fix, `TestCompiler_ConditionalLiteralReturn` **failed both modes** at
  pre-shutdown registry emptiness, 2.450s (group 2.02s).
- `zig build`: **passed**, exit 0. Initial focused test **passed both modes**,
  2.730s. New/P0038/P0037 exact-emission checks **all passed both modes**, 6.967s.
- First full suite: **166 discovered, 147 passed, 1 failed, 18 skipped, zero errors**,
  42.15s; `test_results/test_results_20261115_060804_windows.json`. The runner strips
  trailing whitespace, losing the fixture's final blank line. Moved the checked blank
  line before final backing output; all 34 lines and exact Go assertions remain.
- Final `zig build test`: **166 discovered, 148 passed, 18 skipped, zero failures/
  errors**, 42.01s; `test_results/test_results_20261115_060924_windows.json`.
- Python runner unit tests: **5 passed**, 0.048s.
- Supported Windows Go quick protocol: **all 32 groups passed**, 52.655s; new
  group **1.94s**, both modes including final output ordering and exact C checks.
  Each requires **55 nonnull releases** (18 classes, 37 strings), **34 output lines**,
  selected-only once-only effects, and an empty registry before/after shutdown.
  Native UBSan/live release/reference-count checks, Linux harness cross-linking and
  directly linked output pass. Earlier ownership/shutdown assertions remain intact;
  external compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-building/cross-linking is not native P0039 Linux
  runtime evidence; inspect CI after publication.
- **166 local Markdown paths resolved**, 32-group quick/CI selector equality,
  PowerShell BOM, gofmt, scope/diff review and `git diff --check` passed. Precommit
  fetch/remote SHA matched the parent; publication requires matching remote SHA.

Same eighteen skips and prior shape/operator limits. Literal-only conditional trees
need no result allocation, copy, retain or consumption. Any nonliteral leaf leaves
the entire return on its prior path; caller declarations still copy. Direct owned-
local results retain their P0037 lifetime until shutdown. General result ownership,
async/active exception exits and aliases/containers/destructors/fields remain separate.
See [P0039](passes/P0039-conditional-literal-return.md). Next verify publication/native
CI, then design selected owned-local preservation on conditional returns.

---

# Historical P0038 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC

**Pass:** P0038 - cleanup before direct literal string returns

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0037 publication matched parent `9803ea8626d180e24b17a6a65d638737a5c95cd6`;
  [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43695861891)
  passed Linux/Windows build, runner, quick and full-suite steps before work.
- Restricted fetch failed DNS; restricted baseline could not locate Zig's installed
  directory. Approved normal network/toolchain access succeeded without changing
  toolchain, caches or permissions.
- Baseline `zig build test`: **164 discovered, 146 passed, 18 skipped, zero
  failures/errors**, 49.46s; `test_results_20261115_021139_windows.json`.
- Before fix, `TestCompiler_LiteralStringReturn` **failed both modes** at the
  empty-registry-before-shutdown assertion, 2.479s (group 2.00s).
- `zig build`: **passed**, exit 0. Initial focused new/P0037 groups **passed both
  modes**, 5.930s combined. A strengthened exact C assertion initially included
  debug directives between `if` and its block; correcting the match boundary kept
  both runtime branches checked. Final focused exact C run **passed both modes**,
  3.087s (group 2.61s), with native UBSan/live registry/refcounts/shutdown,
  Linux harness cross-linking and direct linked output.
- `zig build test`: **165 discovered, 147 passed, 18 skipped, zero failures/
  errors**, 52.12s; `test_results/test_results_20261115_021549_windows.json`.
- Python runner unit tests: **5 passed**, 0.035s.
- Supported Windows Go quick protocol: **all 31 groups passed**, 59.603s;
  new group **2.12s**, both modes including exact C checks. Each requires
  **51 nonnull releases** (18 classes, 33 strings), **19 output lines**, and an
  empty registry before and after shutdown. Earlier assertions remain intact.
  External compiler invocations use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-link success is not native P0038
  Linux execution evidence; inspect CI after publication.
- **161 local Markdown paths resolved**, 31-group quick/CI selector equality,
  PowerShell BOM, gofmt, scope/diff review and `git diff --check` checked before
  commit. Precommit fetch/remote SHA matched the parent; no divergence. Publication
  requires a matching remote SHA.

Same eighteen skips and prior shape/operator limits. Literal static results need no
allocation, retain or consumption; caller declarations still copy. Direct owned-
local results retain their P0037 lifetime until shutdown. Borrowed/compound results,
pointer/class results, aliases/containers/destructors/fields, async and active
exception exits remain separate. See [P0038](passes/P0038-literal-string-return.md).
Next verify publication/native CI, then qualify conditional string returns with
all-literal value branches and once-only condition evaluation.

---

## Historical P0037 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC

**Pass:** P0037 - cleanup around direct owned-local string returns

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0036 publication matched parent `43772c74fe258d24d1389e6d8e50857dc0202297`;
  [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43694034403)
  passed Linux/Windows build, runner, quick and full-suite steps before work.
- Restricted fetch failed DNS; restricted baseline could not locate the installed
  Zig directory. Approved normal network/toolchain access succeeded without changing
  toolchain, caches or permissions.
- Baseline `zig build test`: **163 discovered, 145 passed, 18 skipped, zero
  failures/errors**, 51.23s; `test_results_20261115_013451_windows.json`.
- Before fix, `TestCompiler_LocalStringReturn` **failed both modes**, 3.033s
  (group 2.44s), with **44 live allocations versus 17 expected** before shutdown.
- Initial development build failed due to a misplaced return-case scope; source
  correction restored the build. Final `zig build`: **passed**, exit 0. Initial
  focused matrix **passed both modes**, 3.025s (group 2.54s), with native UBSan,
  live registry/refcounts/shutdown, Linux harness cross-linking and linked output.
- `zig build test`: **164 discovered, 146 passed, 18 skipped, zero failures/
  errors**, 50.76s; `test_results/test_results_20261115_013843_windows.json`.
- Python runner unit tests: **5 passed**, 0.033s.
- Supported Windows Go quick protocol: **all 30 groups passed**, 55.520s;
  new group **2.18s**, both modes including final exact C assertions. Each requires
  **45 nonnull releases** (20 classes, 25 strings), **21 output lines**, exactly
  **17 live returned allocations with one reference each** before shutdown, and
  an empty registry afterward. Earlier empty-before-shutdown assertions remain.
  External compiler invocations use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-link success is not native P0037
  Linux execution evidence; inspect CI after publication.
- **156 local Markdown paths resolved**, 30-group quick/CI selector equality,
  PowerShell BOM, gofmt, scope/diff review and `git diff --check` checked before
  commit. Precommit fetch/remote SHA matched the parent; no divergence. Publication
  requires a matching remote SHA.

Same eighteen skips and prior shape/operator limits. Returned owned-local storage
retains its existing lifetime until shutdown; caller declarations still copy it
without consuming the original. Other string result expressions, borrowed-source
lifetime, aliases/containers/destructors/fields, pointer/class results, async and
active exception exits remain separate. See [P0037](passes/P0037-local-string-return.md).
Next verify publication/native CI, then qualify direct literal string-return cleanup.

---

## Historical P0036 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC

**Pass:** P0036 - scoped cleanup on primitive scalar return

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0035 publication matched parent `1dce40c55e4b9166fa73c28b9435c9b1d49997cd`;
  [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43689021933)
  passed Linux and Windows build, runner, quick and full-suite steps before work.
- Restricted fetch failed DNS; restricted baseline could not locate Zig's installed
  directory. Approved normal network/toolchain access succeeded without toolchain,
  cache or permission changes.
- Baseline `zig build test`: **162 discovered, 144 passed, 18 skipped, zero
  failures/errors**, 51.34s; `test_results_20261115_011055_windows.json`.
- Before fix, `TestCompiler_ScalarReturnCleanup` **failed both modes** at the empty-
  registry-before-shutdown assertion, 2.796s (group 2.31s). Earlier fixture probes
  hit existing name-conflict/string-index return typing rules; no rules were weakened.
- `zig build`: **passed**, exit 0. Initial focused regression: **both modes passed**,
  3.349s (group 2.86s). Each checks **47 nonnull releases** (21 classes, 26 strings),
  **23 exact output lines**, native UBSan/live registry/refcounts/shutdown,
  pre-shutdown emptiness, Linux harness cross-linking and direct linked output.
- `zig build test`: **163 discovered, 145 passed, 18 skipped, zero failures/
  errors**, 48.85s; `test_results/test_results_20261115_011545_windows.json`.
- Python runner unit tests: **5 passed**, 0.046s.
- Supported Windows Go quick protocol: **all 29 groups passed**, 55.924s; new
  group **2.58s**, both modes including exact evaluation/cleanup/name-collision
  sequences and string-result/active-exception emission controls. Compiler tests
  use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-link success is not native P0036
  Linux runtime evidence; inspect CI after publication.
- **150 local Markdown paths resolved**, 29-group quick/CI selector equality,
  PowerShell BOM, gofmt, scope/diff review and `git diff --check` passed.
  Precommit fetch and remote SHA matched the parent; no divergence.

Same eighteen skips and prior shape/operator limits. String/pointer/class result
transfer, aliases/containers/destructors/fields, async and active exception unwinding
remain separate. String-index return typing remains limited. See
[P0036](passes/P0036-scalar-return-cleanup.md). Next verify publication/native CI,
then design bounded string-result lifetime/cleanup before implementation.

---

## Historical P0035 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC

**Pass:** P0035 - scalar class locals on bare void return

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0034 publication matched parent `d85bfb4b48fd5d560b0ccacbdffbd80f3dcdde26`;
  [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43684568487)
  passed Linux and Windows jobs before implementation.
- Restricted fetch failed DNS; restricted baseline could not find Zig's installed
  directory. Approved normal network/toolchain access succeeded without changing
  toolchain, caches or permissions.
- Baseline `zig build test`: **161 discovered, 143 passed, 18 skipped, zero
  failures/errors**, 56.08s; `test_results_20261115_000721_windows.json`.
- Before fix, focused `TestCompiler_VoidReturnClasses`: **both modes failed**
  the empty-registry-before-shutdown assertion, 2.855s (group 2.30s).
  Earlier fixture probes rejected unsupported class for headers and null initializers.
- `zig build`: **passed**, exit 0. Initial focused test: **both modes passed**,
  3.466s (group 2.91s). Each checks **40 nonnull releases** (30 class instances,
  ten strings), **ten exact output lines**, native UBSan/live registry/refcounts/
  shutdown, empty pre-shutdown registry, Linux harness cross-linking and direct
  linked output. Final exact reverse-order C assertions passed in the quick run.
- `zig build test`: **162 discovered, 144 passed, 18 skipped, zero failures/
  errors**, 48.56s; `test_results/test_results_20261115_001002_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.038s.
- Supported Windows Go quick protocol: **all 28 groups passed**, 59.076s;
  class void-return group **2.67s**, both modes. Compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. No local Linux execution; cross-build/cross-link success
  is not native P0035 runtime evidence.
- **139 local Markdown paths resolved**, 28-group quick/CI selector equality,
  PowerShell BOM, gofmt, scope/
  diff review and `git diff --check` verified before commit. Precommit fetch and
  remote SHA still matched the parent; no divergence.

Same eighteen skips and previous shape/operator limits. Fresh independent scalar
class locals are qualified; class alias/container ownership, destructors/fields,
value-return transfer, async and exception unwinding remain separate. Unused active
exception-return paths have emission checks only. Direct class null initialization
and custom-type for headers remain unsupported. See
[P0035](passes/P0035-void-return-classes.md). Next verify publication/native CI,
then qualify bounded primitive scalar value-return cleanup with once-only evaluation.

---

## Historical P0034 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC (local reports November 14, America/Vancouver)

**Pass:** P0034 - owned scalar strings on bare void return

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0033 publication matched parent `376b74402d038c9d55aa2b790c1facdd785675a6`;
  [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43677590309)
  passed Linux and Windows jobs before implementation.
- Restricted fetch failed DNS and restricted baseline could not locate the Zig
  installation. Normal approved network/toolchain access succeeded without any
  toolchain/cache/permission configuration change.
- Baseline `zig build test`: **160 discovered, 142 passed, 18 skipped, zero
  failures/errors**, 54.54s; `test_results_20261114_230800_windows.json`.
- Before fix, focused `go test -count=1 -v -run
  '^TestCompiler_VoidReturnStrings$' .` from `codex/go`: **both modes failed**
  the empty-registry-before-shutdown assertion, 3.097s (group 2.58s).
- `zig build`: **passed**, exit 0. Initial focused regression: **both modes
  passed**, 3.554s (group 3.01s). Each requires **40 nonnull releases** (39
  strings, one normal class release), **13 exact output lines**, reverse-order
  cleanup, borrowed caller survival, pre-shutdown emptiness, native UBSan/live
  registry/refcounts/shutdown, Linux harness cross-linking and direct linked output.
- Final `zig build test` with synchronous-function gating: **161 discovered,
  143 passed, 18 skipped, zero failures/errors**, 49.14s;
  `test_results/test_results_20261114_231035_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.043s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all 27 groups passed**, 53.977s;
  void-return group **2.38s**, both modes. Compiler invocations use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Linux executables were not run locally;
  cross-building/cross-linking is not native P0034 Linux runtime evidence.
- Quick/CI selector equality, PowerShell BOM, gofmt, **134 local Markdown paths resolved**,
  complete scope/diff review and `git diff --check` checked before commit.
  Precommit fetch and remote SHA still matched the parent; no divergence.

Same eighteen skips and prior shape/operator limits. Value-return transfer,
class-local return cleanup, async and exception/finally unwinding remain
unqualified. Unused active exception-return paths have emission checks only;
normal exception completion followed by a return executes. Class aliases,
containers/destructors/fields and temporary/result ownership remain separate.
See [P0034](passes/P0034-void-return-strings.md). Next: verify publication/native
CI, then qualify fresh independent class locals on bare synchronous void return.

---

## Historical P0033 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC (local reports November 14, America/Vancouver)

**Pass:** P0033 - owned scalar string append

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0032 publication matched parent `4f9584773eac163ad3486811a4094ef353741bd3`;
  [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43674621260)
  passed both Linux and Windows jobs before implementation.
- Baseline `zig build test`: **157 discovered, 139 passed, 18 skipped, zero
  failures/errors**, 48.49s; `test_results_20261114_212342_windows.json`.
- Before fix, both append modes failed checked harness C compilation on invalid
  pointer `+=`, 0.975s. They could not reach runtime lifetime checks.
- Focused `TestCompiler_AppendedStrings`: **both modes and ten rejection cases
  passed**, 3.084s (group 2.65s) before final signature review; final quick group
  **passed**, 2.70s. Each mode checks **49 nonnull releases, 26 exact output lines**,
  helper/copy/jump emission, native UBSan/live registry/refcounts/shutdown,
  self/nested updates, null-as-empty policy, source/copy lifetime, once-only effects,
  borrowed/member/fresh operands, methods, loops, headers and numeric preservation.
  Ten positioned/artifact-free cases reject const append and non-string operands.
  Linux harness cross-linking and direct compiler-linked output also pass.
- Initial post-change full suite: **160 discovered, 142 passed, 18 skipped, zero
  failures/errors**, 51.07s; `test_results_20261114_213126_windows.json`.
- Final `zig build test` after matching the helper slot to generated `const char *`:
  **160 discovered, 142 passed, 18 skipped, zero failures/errors**, 47.29s;
  `test_results/test_results_20261114_213348_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.055s.
- Supported Windows Go quick protocol: **26 groups passed**, 50.863s initially;
  **all 26 passed again** after signature review, 50.744s. Compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed initially and after signature review**, final **15/15 steps**.
  Linux executables were not run locally; native P0033 CI awaits publication.
- **146 local Markdown paths resolved**; quick/CI selector equality, PowerShell BOM, gofmt,
  complete scope/diff review and `git diff --check` checked before commit.
  Nine pre-existing links in the changed operator guide were corrected to real pages.

Same eighteen skips. Parameter/field/index destination ownership, pointer/array
and conditional operands, other compound operators, owning containers, class
aliases/destructors/fields and return/exception/finally unwinding remain unqualified.
Fresh managed operands can remain registered until shutdown; temporary ownership
transfer and new allocation failure/size policies are not added. See
[P0033](passes/P0033-owned-string-append.md) for intermediate fixture failures and limits.
Next: verify publication/native CI, then T011 owned scalar cleanup on bare void return.

---

## Historical P0032 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC (local reports November 14, America/Vancouver)

**Pass:** P0032 - scalar string assignment-initializer ownership

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0031 publication matched parent `f028fc63f48cbc93723b9a11c3438e7619900189`;
  [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43666265892)
  passed both Linux and Windows jobs before implementation.
- Baseline `zig build test`: **156 discovered, 138 passed, 18 skipped, zero
  failures/errors**, 59.43s; `test_results_20261114_205040_windows.json`.
- Both new modes failed checked live-registry cleanup before the fix. Final
  focused `TestCompiler_AssignmentInitializers`: **both modes passed**, 3.798s;
  group 3.22s. Each checks **17 assignment-copy sites, 30 nonnull releases and
  23 output lines**, native UBSan/registry/refcounts/shutdown, exact generated C,
  once-only call/index effects, Linux harness cross-linking and direct linked output.
- Final `zig build test`: **157 discovered, 139 passed, 18 skipped, zero failures/
  errors**, 47.27s; `test_results/test_results_20261114_205330_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.040s.
- Supported Windows Go quick protocol: **25 groups passed**, 47.174s;
  assignment-initializer group 2.26s. Compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed**. Linux executables were not run locally; native P0032 CI awaits push.
- Local links, quick/CI selector equality, PowerShell BOM, gofmt, scope/diff review
  and `git diff --check` checked before commit.

Same eighteen skips and shape/operator limits. Assignment destination ownership
and temporary ownership transfer remain unchanged; fresh managed operands can
remain registered until shutdown. Compound assignment, owning containers,
class aliases/destructors/fields and return/exception/finally unwinding remain
unqualified. See [P0032](passes/P0032-assignment-initializer-ownership.md).
Next: verify publication/native CI, then T011 owned scalar string `+=`.

---

## Historical P0031 checkpoint - DreamCompiler

**Date:** November 15, 2026 UTC (local reports November 14, America/Vancouver)

**Pass:** P0031 - class-local cleanup on break/continue

**Platform/toolchain:** Windows; Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0030 remote SHA matched parent `d11cc7b280fe1da60122790d1634756a7aac03af`;
  [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43665107774)
  passed Linux and Windows jobs during this run.
- Baseline `zig build test`: **155 discovered, 137 passed, 18 skipped, zero
  failures/errors**, 62.50s; `test_results_20261114_185801_windows.json`.
- Before the fix, both class modes failed the **empty registry before shutdown**
  assertion. Final focused class and existing string jump groups passed, 19.529s;
  class group 3.58s. Each class mode checks **33 nonnull releases** (27 class,
  six strings), **13 output lines**, exact C sequences, UBSan/live registry/refcounts,
  Linux harness cross-linking, pre-shutdown reclamation and direct linked output.
- Final `zig build test`: **156 discovered, 138 passed, 18 skipped, zero failures/
  errors**, 66.91s; `test_results/test_results_20261114_190311_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.061s.
- Supported Windows Go quick protocol: **24 groups passed**, 51.358s; class
  group 2.48s. External compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed**. Linux executables were not run locally; P0031 native CI awaits push.
- Local links, quick/CI selector equality, PowerShell BOM, scope/diff review and
  `git diff --check` checked before commit.

Same 18 skips and shape/operator limits. Class alias/container ownership,
destructors/field destruction, return transfer and exception/finally exits remain
unqualified. See [P0031](passes/P0031-loop-exit-classes.md). Next: verify publication/
native CI, then T011 scalar string assignment-expression initializers.

---

## Historical P0030 checkpoint - DreamCompiler

**Date:** November 14, 2026 (America/Vancouver)

**Pass:** P0030 - conditional scalar string initializer ownership

**Branch/platform:** `codex/compiler-stabilization`, approved local checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- Parent P0029 publication matched `f1d61a75fcfe23c3cdb013809f0f55ffed705aff`;
  [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43661838320) passed
  native Linux and Windows jobs before implementation.
- Baseline: **154 discovered, 136 passed, 18 skipped, zero failures/errors**,
  51.37s. Before the fix both conditional modes failed checked live-registry cleanup.
- Final focused matrix: **both modes passed**, 3.533s, with **19 conditional-copy
  sites, 22 checked nonnull releases and 24 output lines** each. Generated C,
  selected-branch effects, independent lifetime, replacement/jump/header cleanup,
  Linux harness cross-linking, UBSan/refcounts/shutdown and direct output passed.
- `zig build test`: **155 discovered, 137 passed, 18 skipped, zero failures/errors**,
  52.30s; `test_results/test_results_20261114_184830_windows.json`.
- Python runner regressions: **5 passed**, 0.058s.
- Supported Windows Go quick protocol: **all 23 groups passed**, 47.492s,
  including conditional group 2.49s. Compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed**; Linux runtime awaits native CI after publication.
- Changed-doc links, selector consistency, PowerShell BOM, scope/diff review and
  `git diff --check` checked before commit.

Same 18 skips. Typed null-valued branches are covered; literal null/string branch
typing remains rejected. Fresh selected managed results can remain registered
until shutdown; return transfer and class/exception early exits remain separate.
See [P0030](passes/P0030-conditional-string-ownership.md). Next: verify native CI,
then T011 class-instance cleanup on break/continue. Pruning/site tasks are complete.

---

## Historical P0029 checkpoint - November 15, 2026 UTC

**Date:** November 15, 2026 UTC

**Pass:** P0029 - deployed documentation release verification

**Scope/platform:** documentation/evidence only, Windows; public Pages HTTPS reads

- P0028 integration publication verified: fetched branch and `git ls-remote`
  matched `9edc4b9ba0eb636e091ba9a3df9d4e661646d2e5`.
- [PR #231](https://github.com/Ddemon26/DreamCompiler/pull/231) was already merged
  outside this run; merge `6b34823f641bdb4585d7f415b6a08acd0074a791`.
  Pages reports **built, legacy, `main:/docs`**. Its
  [deployment](https://github.com/Ddemon26/DreamCompiler/actions/runs/43660436390)
  build/report/deploy jobs **passed**.
- Native Linux/Windows [P0028 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43658070457)
  and [merged-main CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43660437950)
  jobs **passed**, including build, runner, quick and Dream suite steps.
- `node --check docs/site.js`: **passed**.
- `node --test docs/site.test.mjs`: **8 passed, 0 failed/skipped**, 118.474 ms.
- Live HTTPS verification at **01:06:30 UTC**: **27 HTTP 200 responses** matched
  exact P0028 Git blob bytes: homepage, stylesheet, four JavaScript modules and
  **all 21 sidebar Markdown documents**. Module/CSS MIME and quick-start query/
  deep-link shell assertions **passed**. Merge `docs/` tree matched P0028.
- No browser/app was available to automation. **Live browser rendering and
  interactions were not rerun**. P0028's dated local browser evidence remains
  separate; HTTP success alone does not qualify deployed interactions.
- No compiler/runtime/test/build inputs changed. Local full compiler, Go,
  runner and cross-build **not rerun**. Reused **P0027 / November 14, 2026 UTC**:
  **136 Dream passes, 18 skips, no failures**, **22 quick groups**, **5 runner tests**.
  No skipped fixtures or legacy tutorial/API claims were requalified.
- **93 local Markdown links resolved** across six revised/new documents;
  **408 protected tracked compiler/test/plugin/build/CI inputs unchanged**.
  Change scope, referenced local paths, full diff and `git diff --check` passed.

P0028's release gate is resolved. P0029 changes only documentation/evidence on
the approved integration branch; no merge or Pages-setting change was performed.
See [P0029](passes/P0029-docs-release-verification.md) for evidence and handoff.

---

## Historical P0028 checkpoint - November 14, 2026

**Date:** November 14, 2026 (America/Vancouver)

**Pass:** P0028 - square-edged homepage and documentation reader

**Scope/platform:** static documentation website, Windows; compiler inputs unchanged

- `node --check docs/site.js`: **passed**.
- `node --test docs/site.test.mjs`: **8 passed, 0 failed/skipped**.
- Actual browser rendering of **all 21 sidebar documents passed**. Homepage example
  selection by click/keyboard, search including no results, internal navigation,
  browser back, heading/deep-link reload, clipboard success, saved light theme and
  restored dark theme, archives and 404/retry recovery passed.
- Mobile **320px** homepage/table article and tablet **768px** homepage had no
  page horizontal overflow. Mobile menu and table containment passed. Desktop
  **1280px** homepage and article visually checked; viewport reset afterward.
- Temporary Markdown QA: script/event-handler/unsafe-link removal, no injected
  DOM attribute, distinct duplicate heading IDs and table wrapping passed. The
  temporary fixture was removed before commit.
- Browser initially exposed Windows Python serving `.mjs` as text/plain; runtime
  modules now use `.js`. Subsequent module loading and interactions passed.
- `zig version`: **0.17.1**. The two homepage examples were extracted into ignored
  `build/P0028-example-{0,1}.dr`, compiled through **`zig build run -- ...`** and
  executed sequentially: exact **`Hello, Dream!`** and **`8`**, exit zero. Initial
  direct compiler invocations failed launching default GCC; the documented Zig
  entry point sets `CC=zig cc` and passed.
- **408 protected hashes unchanged**: compiler/runtime, fixtures, runner/Go tooling,
  plugins, build graph and CI. **48 new/revised guide links resolved** before pass
  metadata; final handoff/index links and diff checks precede commit.
- Reused compiler evidence, **P0027 / November 14, 2026 UTC**, not rerun here:
  **136 Dream passes, 18 skips, no failures**, **22 quick groups**, **5 runner tests**.
  Both exact-parent native Linux/Windows [CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/43654089255)
  passed before edits. No new cross-build or native Linux runtime claim.
- Pages metadata read: **built**, **legacy source `main:/docs`**. No live deployment
  performed. Integration publication requires verified push; live release requires
  explicit owner instruction before a `main` merge or Pages-setting change.

No compiler behavior or skipped cases were requalified. Legacy tutorial/API accuracy,
browser forward and disabled-script/storage/clipboard environments were not separately
tested. See [P0028](passes/P0028-docs-website.md) for scope and handoff.

---

## Historical P0027 checkpoint - November 14, 2026

**Date:** November 14, 2026 UTC

**Pass:** P0027 - unsupported Go build entry-point cleanup

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0026 [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43652638299):
  native Linux/Windows jobs **passed** before edits began.
- **474 protected hashes unchanged**, including compiler, fixtures, docs/Pages,
  plugins, unique Go test/legacy sources and supported build/runner/CI inputs.
- Both launchers and CI preserve the same **22-group selector** and `-count=1`.
- Controlled launcher probes: **seven passed**, including default/help, build/test
  exit propagation, directories, retired action and missing prerequisite behavior.
  PowerShell parsing/BOM and GNU Make help/default/explicit dry-runs passed;
  retired Make target rejection passed. Native Linux Make was not executed here.
- `zig build test`: **154 discovered, 136 passed, 18 skipped, zero failures/errors**,
  48.74s; `test_results/test_results_20261114_163145_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.029s.
- Pruned Windows Go quick launcher: **all twenty-two groups passed**, 36.863s.
- Removed-entry searches, **95 local Markdown links**, paths and diff checked before commit. No C/build
  input changed; no cross-build was required or rerun. Native P0027 CI awaits
  publication. Prior P0026 native evidence is dated above.

Compiler checks ran sequentially with approved SDK/cache access and no content or
permission changes. Same 18 skips and prior language/ownership limits remain.
Legacy unique cgo sources/checksums and the active Pages assets remain; no coverage
is claimed for them. See [P0027](passes/P0027-go-tooling-cleanup.md).
Ignored evidence: `build/P0027-*`.

---

## Historical P0026 checkpoint - November 14, 2026

**Date:** November 14, 2026 UTC

**Pass:** P0026 - obsolete tooling and operational documentation cleanup

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0025 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43649932934):
  Linux/Windows jobs and suites **passed** for the exact parent SHA.
- Before/after cleanup: **367 protected source/build/fixture/plugin/runner hashes
  unchanged**; `python codex/test_cli.py list` returns the same **154 entries and
  categories**. CLI `--help` passed. No tests or exclusions were removed.
- `zig build test`: **154 discovered, 136 passed, 18 skipped, zero failures/errors**,
  51.72s; `test_results/test_results_20261114_161430_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.044s.
- Windows Go quick protocol: **all twenty-two groups passed**, 41.955s;
  external compiler tests use `-count=1`.
- Revised operational docs: **101 local links resolved** before pass metadata,
  **184** across all revised/new Markdown in the final check;
  added handoff links, literal paths, inbound fragments, retired-entry references,
  the installer ignore rule and diff were also checked.
- No C/build inputs changed: no new cross-build was required or claimed. Native
  P0026 CI awaits publication. Prior P0025 native evidence is dated above.

Compiler checks ran sequentially with approved SDK/cache access and no SDK/cache
changes. Same 18 skips and existing language/ownership limits remain. Retired
scripts had no active consumers; the supported runner is unchanged. See
[P0026](passes/P0026-repository-cleanup.md) for the deletion inventory and retained
legacy/experimental boundaries. Ignored evidence: `build/P0026-*`.

---

## Historical P0025 checkpoint - November 14, 2026

**Date:** November 14, 2026 UTC

**Pass:** P0025 - first production compiler dogfooding (Dream source normalizer)

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0024 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43647310315):
  Linux/Windows checks and suites **passed** for the exact parent SHA.
- Baseline `zig build test`: **154 discovered, 136 passed, 18 skipped, zero
  failures/errors**, 47.17s; `test_results/test_results_20261114_153454_windows.json`.
- Pre-rebuild `TestCompiler_Dogfood`: **failed as expected**, 0.536s; old compiler
  did not support the library-emission request.
- `zig build`: **passed**, including C-only host bootstrap, emitted Dream helper
  and production compiler link without the C seed.
- Initial harness assumed idempotence incorrectly; the unchanged seed also maps
  CRCRLF to CRLF, then LF. Repeated-call seed equivalence replaced that assumption;
  independent expected bytes, complete-buffer and boundary assertions remain.
- Focused `go test -count=1 -v -run '^TestCompiler_Dogfood$' .`: **passed**, 2.747s,
  including 97,912 native cases and Linux cross-link. Expanded final forward-call,
  rejection/output-preservation and production-newline checks passed in quick.
- Final `zig build test`: **154 discovered, 136 passed, 18 skipped, zero
  failures/errors**, 46.69s; `test_results/test_results_20261114_154100_windows.json`.
- Windows Go quick protocol: **all twenty-two groups passed**, 45.446s; new group
  1.62s. External compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.058s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**, with a
  Windows host bootstrap and Linux production compiler. Linux binaries were not
  executed locally; native P0025 runtime awaits CI.

Same 18 skips/exclusions. This qualifies one production helper and function-only
library emission, not a self-hosted lexer/parser/emitter, general C FFI, new ownership
contracts or whole-compiler stage-two/stage-three equality. Prior T011/shape/operator
limits remain. Compiler checks ran sequentially with approved SDK/cache access and
no SDK/cache changes. See [P0025](passes/P0025-compiler-dogfooding.md).
Ignored logs: `build/P0025-*.log`.

---

## Historical P0024 checkpoint - November 14, 2026

**Date:** November 14, 2026 UTC

**Pass:** P0024 - owned scalar string cleanup on break/continue

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0023 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43630813439):
  Linux/Windows checks and full suites **passed** for the exact parent SHA.
- Baseline `zig build test`: **153 discovered, 135 passed, 18 skipped, zero
  failures/errors**, 43.62s; `test_results/test_results_20261114_150829_windows.json`.
- Before implementation, `TestCompiler_LoopExitStrings` **failed both modes** at
  the exact release total before shutdown, 2.676s. Linux harness cross-links passed
  before native execution; skipped body cleanup deferred storage until shutdown.
- `zig build`: **passed** after target-aware cleanup; initial new group **passed
  both modes**, 3.144s.
- Final focused ownership matrix: **all twenty modes passed**, 24.888s; new group
  body 2.37s. Exact generated-C cleanup sequences, Linux harness cross-links,
  checked native lifetime and exact direct-linked output passed. The new fixture
  requires thirty-seven nonnull releases and sixteen output lines per mode;
  prior allocator/refcount/shutdown assertions and eighteen modes are unchanged.
- Final `zig build test`: **154 discovered, 136 passed, 18 skipped, zero
  failures/errors**, 42.93s; `test_results/test_results_20261114_151212_windows.json`.
- Windows Go quick protocol: **all twenty-one groups passed**, 35.530s;
  external compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.043s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0024 runtime awaits CI.

Compiler checks ran sequentially with approved SDK/cache access after restricted
SDK discovery failed, without content/permission changes. Same 18 unannotated
skips/exclusions. Jump cleanup releases qualified owned strings in exited scopes,
leaving outer locals and for headers live through continue/update. Class-instance
early exits, unsupported initializers, compound assignment, parameter/field/index
destinations, owning containers, return ownership and return/exception/finally
unwinding remain unqualified. See [P0024](passes/P0024-loop-exit-strings.md).
Ignored evidence: `build/P0024-*.log`.

---

## Historical P0023 checkpoint - November 14, 2026

**Date:** November 14, 2026 UTC

**Pass:** P0023 - borrowed string array/pointer block cleanup

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0022 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43598710643):
  Linux/Windows checks and full suite **passed** for the exact parent SHA.
- Baseline `zig build test`: **151 discovered, 133 passed, 18 skipped, zero
  failures/errors**, 34.07s; `test_results/test_results_20261114_120336_windows.json`.
- Before implementation, `TestCompiler_StringContainers` **failed all four
  array/pointer ordinary/module-entry modes** at the live-allocation release
  assertion, 3.717s. Linux harness cross-links passed before native execution.
- `zig build`: **passed** after normal block cleanup began checking existing
  string binding indirection metadata. Owned scalar/class releases remain intact.
- Focused ownership matrix: **all eighteen modes passed**, 18.221s; new group body
  3.93s. Generated-C positive/negative checks, Linux cross-links, checked native
  lifetime and exact direct-linked output passed. New array/pointer fixtures
  require eight/seven nonnull releases and thirteen/eleven output lines per mode;
  prior allocator/refcount/shutdown assertions are unchanged.
- Final `zig build test`: **153 discovered, 135 passed, 18 skipped, zero
  failures/errors**, 35.23s; `test_results/test_results_20261114_120718_windows.json`.
- Windows Go quick protocol: **all twenty groups passed**, 32.398s;
  external compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.032s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0023 runtime awaits CI.

Compiler tests ran sequentially with approved SDK/cache access after restricted
SDK discovery failed, without content/permission changes. Same 18 unannotated
skips/exclusions. String arrays and explicit pointers borrow their elements/targets;
backing must outlive alias use. Owning containers, automatic element destruction,
other scalar initializers, compound assignment, parameter/field/index destination
ownership, return transfer and early-exit cleanup remain unqualified. See
[P0023](passes/P0023-borrowed-string-containers.md). Ignored evidence:
`build/P0023-*.log`.

---

## Historical P0022 checkpoint - November 14, 2026

**Date:** November 14, 2026 UTC

**Pass:** P0022 - owned scalar string replacement copies and old-value release

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0021 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43569739018):
  Linux/Windows checks and full suite **passed** for the exact parent SHA.
- Baseline `zig build test`: **150 discovered, 132 passed, 18 skipped, zero
  failures/errors**, 37.38s; `test_results/test_results_20261114_060342_windows.json`.
- Before implementation, focused replacement tests **failed both modes** on invalid
  live-allocation cleanup, 2.438s. Linux harness cross-links passed first.
- `zig build`: **passed** after replacement repair. Initial fixture adjustments
  replaced unsupported direct null initialization with a typed borrowed-null value
  and moved array backing outside local cleanup, whose separate scalar-style
  release remains a known next task. The old call fixture now requires fifteen
  releases, including reclamation of its replaced top-level backing.
- Final focused ownership check: **all fourteen modes passed**, 14.707s;
  assignment body 2.00s. Generated copy/release ordering, null defaults, assignment
  results, Linux cross-links, checked native lifetime and exact linked output
  passed. Existing allocator/refcount/shutdown assertions remain.
- Final `zig build test`: **151 discovered, 133 passed, 18 skipped, zero
  failures/errors**, 33.90s; `test_results/test_results_20261114_060854_windows.json`.
- Windows Go quick protocol: **all nineteen groups passed**, 28.582s;
  external compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.038s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0022 runtime awaits CI.

Compiler checks ran sequentially with approved SDK/cache access after restricted
SDK lookup failed, without content/permission changes. Same 18 unannotated skips
and exclusions. Replacement copies readable input before releasing the current
owned scalar value; temporary managed results can remain registered until shutdown.
Other initializers, compound assignment, parameter/field/index destinations,
array/pointer-local cleanup, return transfer, return/exception and skipped body-local
cleanup remain unqualified. See [P0022](passes/P0022-assigned-string-ownership.md).
Ignored evidence: `build/P0022-*.log`.

---

## Historical P0021 checkpoint - November 14, 2026

**Date:** November 14, 2026 UTC

**Pass:** P0021 - scalar string call-initializer copies

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0020 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43539278761):
  Linux/Windows stability checks and full suite **passed**.
- Baseline `zig build test`: **149 discovered, 131 passed, 18 skipped, zero
  failures/errors**, 34.69s; `test_results/test_results_20261114_000133_windows.json`.
- Before implementation, focused call tests **failed both modes** on invalid
  live-allocation cleanup, 2.511s. Linux harness cross-links passed first.
- `zig build`: **passed** after adding call copying. The empty-result fixture was
  corrected to check output rather than pointer equality against a literal;
  no existing assertion or compiler equality behavior was changed.
- Final focused ownership check: **all twelve modes passed**, 12.745s; call body
  1.97s. Generated copies/releases, Linux cross-links, checked native lifetime and
  exact direct-linked output passed, retaining all previous ownership checks.
- Final `zig build test`: **150 discovered, 132 passed, 18 skipped, zero
  failures/errors**, 33.46s; `test_results/test_results_20261114_000525_windows.json`.
- Windows Go quick protocol: **all eighteen groups passed**, 27.077s.
  Existing groups/assertions remain; external compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.046s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0021 runtime awaits CI.

Compiler checks ran sequentially with approved SDK/cache access after restricted
SDK lookup failed, without content/permission changes. Same 18 unannotated skips
and exclusions. Calls copy readable results without ownership transfer; original
managed results can remain registered until shutdown. Return/exception and skipped
body-local cleanup, reassignment, fields and array/pointer-local cleanup remain
unqualified. See [P0021](passes/P0021-call-string-ownership.md). Ignored evidence:
`build/P0021-*.log`.

---

## Historical P0020 checkpoint - November 14, 2026

**Date:** November 14, 2026 UTC

**Pass:** P0020 - for-header string declaration scope and cleanup

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0019 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43526977205):
  Linux/Windows jobs **passed**, including stability checks and full suite.
- Unchanged `zig build test`: **148 discovered, 130 passed, 18 skipped, zero
  failures/errors**, 40.26s; `test_results/test_results_20261113_180507_windows.json`.
- Before implementation, focused for-header tests **failed both modes** on leaked
  binding redefinition, 0.535s. After semantic scope repair and a successful build,
  both failed Linux harness C compilation on nine out-of-scope cleanup operands,
  1.245s. Runtime lifetime checks could not run before emission repair.
- `zig build`: **passed** after code generation repair.
- Final focused ownership/diagnostic check: **all ten ownership modes and driver
  diagnostics passed**, 12.118s; for-header body 2.15s. Generated-C copy/cleanup,
  Linux cross-links, native checked lifetime, exact direct-linked output and four
  positioned/artifact-free scope rejections passed.
- Final `zig build test`: **149 discovered, 131 passed, 18 skipped, zero
  failures/errors**, 33.93s; `test_results/test_results_20261113_180919_windows.json`.
- Windows Go quick protocol: **all seventeen groups passed**, 25.622s; for-header
  body 2.06s. Existing groups/assertions remain; compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.043s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0020 runtime awaits CI.

Compiler checks ran sequentially with approved SDK/cache access after restricted
SDK lookup failed, without content/permission changes. Same 18 unannotated skips
and exclusions. Return/exception exits can bypass header cleanup; owned body locals
skipped by break/continue, call/other initializers, reassignment, return/field
ownership and array/pointer-local cleanup remain unqualified. See
[P0020](passes/P0020-for-header-string-ownership.md). Ignored evidence: `build/P0020-*.log`.

---

## Historical P0019 checkpoint - November 13, 2026

**Date:** November 13, 2026 UTC

**Pass:** P0019 - scalar member string declaration ownership

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0018 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43515951800):
  Linux and Windows jobs **passed**, including stability checks and full suite.
- Unchanged `zig build test`: **147 discovered, 129 passed, 18 skipped, zero
  failures/errors**, 39.15s; `test_results/test_results_20261113_161704_windows.json`.
- Pre-change `go test -count=1 -v -run '^TestCompiler_MemberStrings$' .`:
  **both modes failed** the live-allocation cleanup assertion, 3.405s (body 2.84s).
  Linux harness cross-links passed before native execution failed.
- `zig build`: **passed** after implementation.
- `go test -count=1 -v -run
  '^TestCompiler_(MemberStrings|IdentifierStrings|LiteralStrings|IndexedStrings)$' .`:
  **all eight modes passed**, 11.646s; generated-C, Linux cross-link, native checked
  lifetime and exact output through the harness and direct linker.
- Final `zig build test`: **148 discovered, 130 passed, 18 skipped, zero
  failures/errors**, 36.98s; `test_results/test_results_20261113_162034_windows.json`.
- Windows Go quick protocol: **all sixteen groups passed**, 29.766s; member
  body 3.08s. Existing groups/assertions remain selected; external tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.043s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0019 runtime awaits CI.

Compiler checks ran sequentially with approved SDK/cache access after restricted
SDK lookup failed, without content/permission changes. Same 18 unannotated skips
and exclusions. Call/for-header initializers, reassignment, return/field ownership,
array/pointer-local and early-exit cleanup remain unqualified. See
[P0019](passes/P0019-member-string-ownership.md). Ignored evidence: `build/P0019-*.log`.

---

## Historical P0018 checkpoint - November 13, 2026

**Date:** November 13, 2026 UTC

**Pass:** P0018 - scalar identifier string declaration ownership

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0017 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43489193804):
  Linux and Windows jobs **passed**, including full suite and stability checks.
- Unchanged `zig build test`: **146 discovered, 128 passed, 18 skipped, zero
  failures/errors**, 41.44s; `test_results/test_results_20261113_145511_windows.json`.
- Pre-change `go test -count=1 -v -run '^TestCompiler_IdentifierStrings$' .`:
  **both modes failed** the live-allocation cleanup assertion, 2.703s (body 2.25s).
  Linux harness cross-links passed before native execution failed.
- `zig build`: **passed** after implementation.
- `go test -count=1 -v -run
  '^TestCompiler_(IdentifierStrings|LiteralStrings|IndexedStrings)$' .`:
  **all six modes passed**, 9.195s; generated-C, Linux cross-link, native checked
  allocation lifetime and exact output through the harness and direct linker.
- Final `zig build test`: **147 discovered, 129 passed, 18 skipped, zero
  failures/errors**, 47.33s; `test_results/test_results_20261113_145812_windows.json`.
- Windows Go quick protocol: **all fifteen groups passed**, 23.940s; identifier
  body 2.41s. Existing checks remain selected; external tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.047s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0018 runtime awaits CI.

Compiler checks ran sequentially with approved SDK/cache access after restricted
SDK lookup failed, without content/permission changes. Same 18 unannotated skips
and exclusions. Call/member/for-header initializers, reassignment, return/field
ownership and early-exit cleanup remain unqualified. See
[P0018](passes/P0018-identifier-string-ownership.md). Ignored evidence:
`build/P0018-*.log`.

---

## Historical P0017 checkpoint - November 13, 2026

**Date:** November 13, 2026 UTC

**Pass:** P0017 - literal string declaration ownership

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0016 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43431180404):
  Linux and Windows jobs **passed**, including full suite and stability checks.
- Unchanged `zig build test`: **145 discovered, 127 passed, 18 skipped, zero
  failures/errors**, 33.24s; `test_results/test_results_20261113_120005_windows.json`.
- Pre-change `go test -count=1 -v -run '^TestCompiler_LiteralStrings$' .`:
  **both modes failed** the checked live-allocation assertion, 2.367s.
  An unrelated direct-null fixture declaration was removed before reproduction.
- `zig build`: **passed**. First focused post-change run caught omitted existing
  parentheses in a new C-emission assertion; corrected that assertion.
- `go test -count=1 -v -run '^TestCompiler_(LiteralStrings|IndexedStrings)$' .`:
  **four modes passed**, 5.214s; emitted C, Linux cross-links, checked lifetime,
  exact output from both harness and direct compiler linking.
- Final `zig build test`: **146 discovered, 128 passed, 18 skipped, zero failures/
  errors**, 36.68s; `test_results/test_results_20261113_120323_windows.json`.
- Windows Go quick protocol: **all fourteen groups passed**, 18.349s; literal
  group 2.05s, indexed group 1.91s, including final single-reference assertion.
  Existing groups/assertions remain selected; external tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.056s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0017 runtime awaits CI.

Compiler checks ran sequentially with approved SDK/cache access after restricted
SDK lookup failed, without content/permission changes. Same 18 unannotated skips
and exclusions; identifier/call/for-header/alias/reassignment/return/early-exit/field
ownership remain unqualified. See [P0017](passes/P0017-literal-string-ownership.md).
Ignored evidence: `build/P0017-*.log`.

---

## Historical P0016 checkpoint - November 13, 2026

**Date:** November 13, 2026 UTC

**Pass:** P0016 - preserve indexed primitive console formats

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- P0015 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43389722344):
  Windows and Linux jobs **passed**, including stability checks and full suite.
- Unchanged `zig build test`: **144 discovered, 126 passed, 18 skipped, zero
  failures/errors**, 31.41s; `test_results/test_results_20261113_060253_windows.json`.
- Focused pre-change backend reproduction: **both modes failed ten format checks**,
  0.466s. Prior unsupported shadowing in the new fixture was corrected separately.
- After implementation, `go test -count=1 -v -run '^TestCompiler_IndexedConsole$' .`
  from `codex/go`: **both modes passed**, 1.218s; generated-C and 41 output lines.
- Final `zig build test`: **145 discovered, 127 passed, 18 skipped, zero failures/
  errors**, 29.57s; `test_results/test_results_20261113_060617_windows.json`.
- Windows Go quick protocol: **all thirteen groups passed**, 15.509s; indexed
  console group 0.63s, final 18 generated format checks and exact output per mode.
  All previous groups/assertions remain selected; external tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.041s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not run locally; native P0016 runtime awaits CI.

Compiler checks ran sequentially. Normal approved SDK/cache access used after
restricted Zig lookup failed; no SDK/cache content or permission changes. Same
18 unannotated skips/exclusions remain. Backend formatting does not qualify
semantic shapes, bounds, null safety, general operators or broader ownership.
See [P0016](passes/P0016-indexed-console.md). Ignored logs: `build/P0016-*.log`.

---

## Historical P0015 checkpoint - November 13, 2026

**Date:** November 13, 2026 UTC

**Pass:** P0015 - restore Linux indexed-string harness builds

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- Prior P0014 [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43326685373):
  Linux failed on undeclared `strdup` in both harness builds; Windows cancelled.
- Linux C11 object compilation and both newly added harness cross-links reproduced
  the same four errors before adding `_GNU_SOURCE` to match `build.zig`.
- Focused `go test -count=1 -v -run '^TestCompiler_IndexedStrings$' .`: **both modes
  passed** Linux cross-link, native checked lifetime and exact direct-linked output,
  2.846s (body 2.39s).
- Final isolated `zig build test`: **144 discovered, 126 passed, 18 skipped, zero
  failures/errors**, 34.21s; `test_results/test_results_20261113_005631_windows.json`.
- Windows Go quick protocol: **all twelve groups passed**, 17.419s; indexed-string
  group 2.19s. All previous assertions/groups remain; external tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.045s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux compiler/harness binaries were not run locally; native P0015 CI awaits push.

Initial baseline reported 126 passes/18 skips/no failures, 38.93s, but briefly
shared generated output with the focused reproduction; final isolated validation
above supersedes it. See [P0015](passes/P0015-linux-string-harness.md) for exact
commands, setup errors and limits. Approved SDK/cache access used without content
or permission changes. Same 18 unannotated skips and exclusions; no language or
ownership behavior changes. Ignored evidence: `build/P0015-*.log`.

---

## Historical P0014 checkpoint - November 12, 2026

**Date:** November 12, 2026 UTC

**Pass:** P0014 - indexed scalar string ownership

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- Unchanged `zig build test`: **143 discovered, 125 passed, 18 skipped, zero
  failures/errors**, 35.84s; `test_results/test_results_20261112_160128_windows.json`.
- Uninstrumented pre-change execution happened to pass; the checked allocator
  reproduced invalid literal cleanup deterministically in ordinary/module-entry
  modes, 1.663s. See the pass record for fixture setup failures and corrections.
- `zig build`: **passed** after implementation.
- Final `go test -count=1 -v -run '^TestCompiler_IndexedStrings$' .`: **two modes
  passed** generated-C, checked allocation lifetime and direct linked exact output,
  2.317s (body 1.82s). Null, nested blocks, literal/dynamic backing, top-level copies
  and once-only indexing included. Helper lifetime survives backing deallocation.
- First full suite: 144 discovered, 125 passed, 18 skipped, **one output failure**,
  zero errors, 32.50s. Fixed new fixture metadata line ordering to the established
  Go oracle; no implementation or runtime assertion changes. Report:
  `test_results/test_results_20261112_160632_windows.json`.
- Final `zig build test`: **144 discovered, 126 passed, 18 skipped, zero failures/
  errors**, 33.45s; `test_results/test_results_20261112_160736_windows.json`.
- Windows Go quick protocol: **all twelve groups passed**, 15.706s; indexed string
  group 1.52s. All existing groups remain selected; external compiler tests use
  `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.040s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0014 runtime awaits CI.
- Full diff review and `git diff --check`, 57 local Markdown link occurrences,
  metadata/Go oracle agreement, Go formatting, quick/CI selection and PowerShell
  UTF-8 BOM checks: **passed**.

Restricted Zig SDK lookup failed; normal approved SDK/cache access passed without
content or permission changes. Restricted fetch failed DNS; approved fetch matched
HEAD. Prior P0013 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43265683253)
passed on Windows and Linux. The same 18 unannotated skips and exclusions remain.
Only indexed scalar string declarations gain copied ownership; other ownership,
shape/lifetime, early-exit and scalar string indexing paths remain unqualified.
See [P0014](passes/P0014-indexed-string-ownership.md), [STATE.md](STATE.md) and the
[ownership contract](../docs/development/string-ownership.md).
Ignored evidence: `build/P0014-*.log`.

---

## Historical P0013 checkpoint - November 12, 2026

**Date:** November 12, 2026 UTC

**Pass:** P0013 - primitive pointer parameter signature emission

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

- Unchanged `zig build test`: **142 discovered, 124 passed, 18 skipped, zero
  failures/errors**, 38.12s; `test_results/test_results_20261112_095735_windows.json`.
- Pre-change `go test -count=1 -v -run '^TestCompiler_PointerParameters$' .`:
  **failed as intended**, 0.483s. All ten pointer signature expectations failed
  in each of ordinary/module-entry emission; no host compilation was attempted.
- `zig build`: **passed** after the three parameter-emission paths were repaired.
- Final focused pointer matrix: **two modes passed**, ten C signature checks per
  mode plus exact runtime output, 1.232s (body 0.80s).
- Final `zig build test`: **143 discovered, 125 passed, 18 skipped, zero failures/
  errors**, 35.34s; `test_results/test_results_20261112_100109_windows.json`.
- Windows Go quick protocol: **all eleven groups passed**, 13.567s; pointer
  group 0.74s. Existing scalar argument/constructor diagnostics, parser recovery,
  modules, driver, loop and literal-emission checks remain selected. External
  compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.037s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; inspect native P0013 CI after push.
- Full diff review, `git diff --check`, all 42 local Markdown link occurrences,
  new parameter anchor, source/fixture paths, Go formatting, quick/CI selection
  and PowerShell UTF-8 BOM checks **passed**.

Initial focused attempts exposed direct indexed float/char console formatting
(`0`/`90` instead of `2.500000`/`Z`) and borrowed indexed string local cleanup
(panic in `dr_release` on misaligned `DrRef`, both modes). Final fixture reads
numeric elements through typed locals and prints the string element directly,
retaining all exact output assertions and all five primitive pointer types.
Module free-function signature expectations now use the actual lowered prefix.
These fixture changes bound the signature objective; they do not fix or qualify
those separate defects. T009/T010 preserve their triggers and acceptance criteria.

Restricted baseline could not locate Zig's installed SDK; approved SDK/cache
access passed all required checks without content/permission/cache changes.
Restricted fetch failed DNS; approved fetch succeeded and matched HEAD. Prior
P0012 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43208505371)
passed. The same 18 unannotated skips and existing exclusions remain unchanged.
Pointer shape, lifetime, bounds, return and async paths remain unqualified.
See [P0013](passes/P0013-pointer-parameter-emission.md) and [STATE.md](STATE.md).
Ignored evidence: `build/P0013-*.log`.

---

## Historical P0012 checkpoint - November 12, 2026

**Date:** November 12, 2026 UTC

**Pass:** P0012 - parser parameter recovery and statement progress

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

## Current measured state

- Unchanged `zig build test`: **141 discovered, 123 passed, 18 skipped, zero
  failures/errors**, 43.58s. Report: `test_results/test_results_20261112_041227_windows.json`.
- Pre-change focused original-trigger regression: **two ten-second timeouts**,
  ordinary and module-entry paths, as expected. Go total 21.348s (body 20.88s).
- `zig build`: **passed** after implementation.
- Final `go test -count=1 -v -run '^TestCompiler_ParserRecovery$' .` from `codex/go`:
  **75 bounded rejections passed**, 1.907s. Covers ordinary/module-entry/dependency
  parsing, useful positions, following declarations and no generated artifacts.
- Final `zig build test`: **142 discovered, 124 passed, 18 skipped, zero failures/
  errors**, 38.82s. New custom-parameter rejection passed. Report:
  `test_results/test_results_20261112_041748_windows.json`.
- Windows Go quick protocol from `codex/go`: **all ten groups passed**, 12.752s.
  Includes 75 parser rejections (0.99s), 80 constructor rejections/two programs,
  82 method rejections/two programs, 38 diagnostics, arena growth, warnings,
  four loop settings, 25 module cases and literal emission/failure checks.
  Compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.044s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not run locally; inspect P0012 native CI after publication.
- Full diff review, `git diff --check`, all 36 local Markdown link occurrences,
  Go formatting, source/regression paths, quick/CI selections and PowerShell BOM
  checks **passed**.

Initial focused failures were test position expectations: the enclosing brace was
on line 4, then EOF name positions differed because the entry driver appends a
newline and the dependency loader does not. The fixtures now explicitly place the
brace on line 3 and EOF on line 4, retaining strict positions and all cases. No
assertion/exclusion was weakened. Restricted baseline could not locate Zig's SDK;
compiler checks used approved SDK/cache access without changing SDK/cache contents
or permissions. Restricted fetch failed DNS; approved fetch succeeded. P0011
[CI run 43022236461](https://github.com/Ddemon26/DreamCompiler/actions/runs/43022236461)
passed native Windows/Linux jobs. No intermittent SDK/cache errors recurred in
this pass's approved baseline or final checks.

The same 18 unannotated skips and existing module/graphics/concurrency exclusions
remain. Custom-type/array parameters and pointer emission/shape compatibility remain
unsupported/unqualified. Recovery does not qualify every malformed construct or
unbounded nesting. See [P0012](passes/P0012-parser-parameter-recovery.md) and
[STATE.md](STATE.md). Publication requires a matching remote SHA. Ignored evidence:
`build/P0012-*.log` includes all attempts and required checks.

---

## Historical P0011 checkpoint - November 11, 2026

**Date:** November 11, 2026 UTC

**Pass:** P0011 - primitive scalar constructor arguments and arity

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

## Current measured state

- Unchanged baseline `zig build test`: **139 discovered, 121 passed, 18 skipped,
  zero failures/errors**, 47.96s; `test_results/test_results_20261111_052841_windows.json`.
- Pre-change constructor float-to-int rejection failed as intended: compiler
  returned 0 and generated C rather than diagnosing the mismatch.
- `zig build`: passed after implementation.
- Focused `go test -count=1 -v -run '^TestCompiler_ConstructorArguments$' .`:
  **80 rejection cases and two exact-output programs passed**, 2.830s (body 2.41s).
- First final `zig build test`: **141 discovered, 122 passed, 18 skipped, zero
  assertion failures, one compile error**, 44.34s. Unchanged `semantics/basic.dr`
  hit a Zig cache FileNotFound opening SDK
  `lib/libc/mingw/lib64/api-ms-win-crt-private-l1-1-0.def`. That path was absent
  when checked. Focused `python codex/test_cli.py run --pattern
  '**/semantics/basic.dr'` then passed **1/1**, 0.80s. Report:
  `test_results/test_results_20261111_053344_windows.json`.
- Full retry: **141 discovered, 122 passed, 18 skipped, zero assertion failures,
  one compile error**, 44.21s. Unchanged `inheritance_basic.dr` hit libzigc
  `file_open Unexpected` on SDK `lib/std/math/big.zig`; that path exists.
  Report: `test_results/test_results_20261111_053501_windows.json`.
- Windows Go quick from `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass
  -File ./build_and_test.ps1 quick`: **all nine groups passed**, 12.690s. Includes
  80 constructor rejections/two exact-output programs (1.85s), 82 method rejections/
  two programs, 38 diagnostic cases, arena growth, warnings, four loop settings,
  25 modules, and literal emission/failure checks. Compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.038s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; inspect native P0011 CI after publication.
- Final unchanged `zig build test` after quick/cross-build: **141 discovered,
  123 passed, 18 skipped, zero failures/errors**, 43.93s. Both new Dream entries
  passed. Report: `test_results/test_results_20261111_053712_windows.json`.
- Full diff review, local links, new constructor anchor, source paths, Go formatting,
  quick/CI selection, PowerShell BOM and `git diff --check` passed. Twelve pre-existing
  broken type-system links are unchanged from the parent.

The earlier full attempts did not pass; the changing SDK/cache failures did not
recur in the final run, and their cause remains unproven. No SDK/cache content or
permissions, assertions, exclusions, implementation or fixtures changed between
full attempts and the final pass. Restricted execution could not locate Zig's SDK;
compiler checks used approved installed-toolchain/cache access. Restricted fetch
failed DNS; approved fetch passed. P0010 Linux/Windows CI run 42816446088 passed.

The initial new focused fixtures exposed unsupported custom-type parameter syntax
that stalls parsing, and module/function/import name collisions. The nested test
now uses the supported primitive expression `new Child(1.5).value`; diagnostic and
once-only assertions remain intact. Positive names were corrected, with no weakened
assertions or exclusions. T007 records parser recovery as the next task.

The same 18 unannotated skips and existing module/graphics/concurrency exclusions
remain. Pointer/array shapes, custom-type compatibility, primitive pointer parameter
emission, field initializers and implicit inherited construction remain unqualified/
unsupported. See [P0011](passes/P0011-constructor-arguments.md) and [STATE.md](STATE.md).
Publication requires the pass commit and matching remote SHA. Ignored evidence:
`build/P0011-*.log`, including baseline, pre-change, focused attempts, full-suite
attempts, quick, Python and cross-build.

---

## Historical P0010 recovery checkpoint - November 10, 2026

**Date:** November 10, 2026 UTC

**Pass:** P0010 - primitive scalar method arguments; validation recovered

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.15, Go 1.27.0

## Current measured state

- `zig build test`: **139 discovered, 121 passed, 18 skipped, zero failures/errors**,
  47.48s. Report: `test_results/test_results_20261110_052829_windows.json`.
  Both new annotated method-argument entries passed.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all eight groups passed**, 10.964s. The protocol
  rebuilds the compiler and uses `-count=1`. It includes **82 method argument
  rejections and two exact-output programs** (2.19s focused group), 38 diagnostic
  cases, arena growth, nonfatal warnings, four loop optimization settings,
  25 module cases, and literal emission/failure checks.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.052s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; inspect native CI after publication.
- Full diff review and `git diff --check` passed. All **29 local link occurrences**
  resolve; 12 legacy broken links in the type-system guide are unchanged from HEAD.
  The method anchor, regression paths, quick/CI selection, PowerShell BOM, and Go
  formatting were verified. Publication requires a matching remote SHA.

This recovers the existing P0010 pass; no new feature or pass ID was started.
Restricted fetch failed DNS; approved fetch passed and the remote still matched
P0009. Restricted `zig build test` could not locate the installed SDK; compiler
checks used approved SDK/cache access. Earlier changing SDK/cache file-open errors
did not recur. Their cause remains unproven. No SDK/cache content or permissions,
tests, assertions, exclusions, or implementation changed during recovery.
Ignored evidence: `build/P0010-recovery-{full-suite,full-suite-approved,go-quick,
python,linux-cross-build}.log`.

The same 18 unannotated entries remain skipped/unverified; supporting module fixtures
and graphics/concurrency exclusions are unchanged. Pointer/array/custom method
argument shapes, implicit constructor checks, and pointer parameter C emission remain
unqualified. See [P0010](passes/P0010-method-arguments.md) and [STATE.md](STATE.md)
for historical failures, scope, and next steps. P0009 CI run 42481471439 remains
successful; P0010 native CI must be inspected after publication.

---

## Historical P0010 implementation checkpoint - November 9, 2026

**Date:** November 9, 2026 UTC

**Pass:** P0010 - primitive scalar method arguments; **validation/delivery blocked**

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.15, Go 1.27.0

## Current measured state

- Unchanged `zig build test`: **137 discovered, 119 passed, 18 skipped, zero
  failures/errors**, 47.31s.
- Pre-change focused Go float-to-int rejection failed: compiler returned 0 and
  created C instead of issuing a Dream argument diagnostic.
- `zig build`: **passed** after implementation.
- Focused `go test -count=1 -v -run '^TestCompiler_MethodArguments$' .`: **80
  rejections and two exact-output programs passed**, 2.925s. Final quick adds a
  nested-invalid-argument case in each mode for **82 rejections**.
- First final `zig build test`: **139 discovered, 120 passed, 18 skipped, zero
  assertion failures, one compile error**, 44.64s. Unchanged `test_debug_simple.dr`
  failed Zig mingw cache `file_open Unexpected` on SDK `misc/__winitenv.c`.
- Referenced SDK file exists. Focused `python codex/test_cli.py run --pattern
  '**/test_debug_simple.dr'`: **1 passed**, no skips/failures/errors, 0.81s.
- Full-suite retry: **139 discovered, 119 passed, 18 skipped, zero assertion
  failures, two compile errors**, 50.37s. Unchanged `string_array.dr` and
  `global_release.dr` hit the same mingw error on SDK `libsrc/power-uuid.c` and
  `gdtoa/g_xfmt.c`; both files exist. **Neither full run passed.** Latest report:
  `test_results/test_results_20261109_053831_windows.json`.
- Windows Go quick protocol: **failed**, 11.233s. Default loop compilation hit
  Zig `libzigc` cache `file_open Unexpected` on SDK `std/zig/string_literal.zig`.
  The `-O1`/`-O2`/`-O3` loop cases passed. Seven other groups passed: 38 diagnostics,
  arena growth, warnings, 25 module cases, literal emission/failures, and method
  arguments (**82 rejections and two exact-output programs**). All external
  compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.045s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; no P0010 commit/native CI exists.

Both new annotated entries passed in both complete runs. The initial new positive
fixture failed because existing `int*` method parameters emit C `int`, so null was
rejected by the host compiler. That deferred pointer assumption was removed from
the scalar fixture; a separate negative case with a pointer parameter and mismatched
second scalar requires exactly one scalar diagnostic. Pointer/array emission and
shape compatibility are not qualified by this pass. No existing test was removed,
excluded, or weakened.

Restricted baseline could not locate Zig's installation; compiler checks used
approved toolchain/cache access. SDK/cache contents and permissions were unchanged.
The changing file-open failures resemble earlier checkpoints but their cause is
unproven. Required full/quick validation must pass before P0010 commit/publication.
Preserve the uncommitted work; recover this pass instead of allocating another ID.
Ignored evidence: `build/P0010-{baseline,baseline-approved,rejection-before,build,
focused-go,focused-go-fixed,full-suite,focused-debug,full-suite-retry,python,
go-quick,linux-cross-build}.log`.

P0009 [CI run 42481471439](https://github.com/Ddemon26/DreamCompiler/actions/runs/42481471439)
passed on Linux and Windows before this pass. The same 18 entry skips, supporting
modules, and graphics/concurrency exclusions remain. See
[P0010](passes/P0010-method-arguments.md) for recovery and scope limits.

---

## Historical P0009 test checkpoint

**Date:** November 8, 2026 UTC

**Pass:** P0009 - checked literal arithmetic reaches emitted C/object code

**Branch/platform:** `codex/compiler-stabilization`, attached primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.15, Go 1.27.0

## Current measured state

- Unchanged baseline `zig build test`: **136 discovered, 118 passed, 18 skipped,
  zero failures/errors**, 42.56s.
- Final `zig build test`: **137 discovered, 119 passed, 18 skipped, zero failures/
  errors**, 39.12s. New annotated literal regression checks 49, 65, 2147483646, 0.
  Report: `test_results/test_results_20261108_053647_windows.json`.
- Focused `python codex/test_cli.py run --pattern '**/literal_arithmetic.dr'`:
  **1 passed**, zero skips/failures/errors, 0.79s.
- Focused Go `go test -count=1 -v -run 'TestCompiler_Literal(Emission|Failures)$' .`
  from `codex/go`: **passed**, 3.508s after the object header fix.
- Windows Go quick protocol from `codex/go`, `powershell -NoProfile -ExecutionPolicy
  Bypass -File ./build_and_test.ps1 quick`: **all seven groups passed**, 9.343s,
  including 38 diagnostic cases, arena growth, warnings, four loop settings,
  25 module cases, literal emission, and literal internal failures.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.046s.
- `zig build` and `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`:
  **passed**. Linux executables were not run locally; P0009 native behavior awaits CI.

The emission matrix inspects generated C before host compilation and then compares
exact output at default/`-O1`/`-O2`/`-O3`. It covers nested/zero/boundary arithmetic,
free functions, methods, imported functions/source mapping, side effects, unsupported
whole-tree fallback, accepted/rejected operation/depth bounds, and object success/
nonzero host failure. Undefined overflow/division-by-zero examples are inspected
only. The UBSan C harness checks controlled SCCP/arena allocation failures, a
transformed-root mismatch, bounded nonconvergence, metadata, and atomic replacement.
Fault controls exist only in test code, with no production bypass paths.

Initial focused checks passed C/runtime settings and the allocation harness but
failed object compilation on missing `../libs/console.h` from temporary C. The common
emitter's header paths and object status propagation were fixed; final focused and
quick checks passed. No test exclusions or assertions were weakened.
Restricted baseline could not access Zig's installation directory; approved installed
toolchain/cache access passed. No SDK/cache repair or deletion was performed.

Ignored logs: `build/P0009-{baseline,baseline-approved,build,focused-go,
focused-go-fixed,focused-dream,full-suite,python,go-quick,linux-cross-build}.log`.

P0008 CI run [42373515160](https://github.com/Ddemon26/DreamCompiler/actions/runs/42373515160)
succeeded on Linux and Windows before this pass. P0009's local Linux cross-build
is separate evidence. The same 18 unannotated entries remain skipped/unverified;
supporting modules and graphics/concurrency exclusions are unchanged. General IR
emission/SSA and loop transformations remain unqualified. See
[P0009](passes/P0009-literal-emission.md) for limits and the delivery handoff.

---

## Historical P0007 test checkpoint

**Date:** November 6, 2026 UTC (local reports also use November 6)

**Pass:** P0007 - optimized loop compilation stability and one checked skipped entry

**Branch/platform:** `codex/compiler-stabilization`, attached primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.15, Go 1.27.0

## Current measured state

- Unchanged baseline `zig build test`: **136 discovered, 117 passed, 19 skipped,
  zero failures/errors** (49.32s). The loop fixture was skipped for missing metadata.
- Final `zig build test`: **136 discovered, 118 passed, 18 skipped, zero failures/
  errors** (42.32s). The same loop source now checks results 112 and 8 at `-O2`.
  Report: `test_results/test_results_20261106_053355_windows.json`.
- Focused `python codex/test_cli.py run --pattern '**/test_loop_opt.dr'`: **1 passed**,
  zero skips/failures/errors (0.85s).
- Focused Go `go test -count=1 -v -run '^TestCompiler_LoopsAtOptimizationLevels$' .`
  from `codex/go`: **default, -O1, -O2, -O3 all passed**, printing exactly 112 and 8
  (7.60s test body, 8.141s Go result).
- Windows Go quick protocol from `codex/go`, `powershell -NoProfile -ExecutionPolicy
  Bypass -File ./build_and_test.ps1 quick`: **all five groups passed**, including
  38 diagnostic cases, arena growth, nonfatal warnings, four loop optimization settings,
  and 25 module cases (`-count=1`, 7.065s).
- `python -m unittest discover -s codex/python -p test_test_manager.py`: **5 passed**
  (0.045s).
- `zig build` and `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally. P0007 native Linux behavior awaits CI.
- Diff review, local handoff links, quick/CI selection, PowerShell BOM preservation,
  and `git diff --check` checked before commit.

The new Go matrix initially failed at `-O1`/`-O2`/`-O3` with heap corruption exit
`0xc0000374`; default succeeded. SCCP negative table indices and LICM shared instruction
ownership were repaired. Intermediate `-O2` runs stalled because LICM/loop analysis
reported changes without transformations; only this run's identified processes were
stopped. An intermediate rebuild hit AccessDenied while its executable was still
running; it passed after that process stopped. The final checks above all passed.
No test exclusions or assertions were weakened. Full details and the remaining 18
unannotated entries are in [P0007](passes/P0007-loop-optimization-stability.md).

Restricted execution could not locate Zig's installation; compiler checks used approved
installed-toolchain/cache access. No SDK/cache repair or deletion was performed. Logs:
`build/P0007-{baseline,build,focused-go,focused-go-sccp,focused-go-fixed,focused-dream,
full-suite,go-quick,python,linux-cross-build}.log`. Intermediate failures are diagnostic
evidence, not successful validation. Optimized IR still does not feed C emission, so
these results do not prove optimizer transformations or performance gains.

Supporting modules remain fixtures; four graphics and four concurrency examples remain
outside the suite. Recursive legacy Go/cgo tests remain unsupported. P0006 CI run
42135769585 succeeded on Linux and Windows before this pass began. P0006 recovery
results were 117 passes, 19 skips, zero failures/errors (46.57s), all four then-current
Go quick groups, five runner tests, and a Linux cross-build; those historical recovery
results are recorded in [P0006](passes/P0006-member-access-diagnostics.md).

## Historical P0006 implementation checks - November 5, 2026

- Unchanged baseline `zig build test`: **133 discovered, 114 passed, 19 skipped,
  0 failed, 0 errors** (41.39s).
- First post-change `zig build test`: **136 discovered, 115 passed, 19 skipped,
  0 assertion failures, 2 compile errors** (37.85s). Unchanged `method.dr` and
  `multi_decl.dr` hit Zig libubsan/cache `file_open Unexpected` errors referencing
  SDK files `lib/std/os/linux/powerpc.zig` and `lib/std/array_hash_map.zig`.
- Full-suite retry: **136 discovered, 116 passed, 19 skipped, 0 assertion failures,
  1 compile error** (37.06s). Unchanged `bool_array.dr` hit a libubsan/cache file-open
  error referencing `lib/std/Build/Step/CheckFile.zig`. Report:
  `test_results/test_results_20261105_053133_windows.json`. **Neither attempt passed.**
- From `codex/go`, `go test -count=1 -v -run
  'TestCompiler_(DiagnosticExitStatus|Modules)$' .`: **38 diagnostic and 25 module
  cases passed**, including useful source locations and no artifacts for rejections.
- Windows Go quick protocol from `codex/go`,
  `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`:
  first run failed in unchanged module case `entry_class` with a mingw-w64 crt2.o
  cache file-open error referencing `lib/include/avx512vlbf16intrin.h`.
  **Retry passed all four groups** (38 diagnostics, arena growth, nonfatal warnings,
  25 modules; compiler tests use `-count=1`).
- `python codex/test_cli.py run --pattern '**/*member*.dr'`: **6 discovered, 6 passed,
  0 skipped, 0 failed, 0 errors** (1.51s).
- `python -m unittest discover -s codex/python -p test_test_manager.py`: **5 passed**.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **succeeded**.
  Linux executables were not run locally. P0005 CI run 41833228164 succeeded;
  P0006 has not been published or checked by CI.
- Diff review, local handoff link checks, and `git diff --check` passed.

Compiler checks used approved installed-toolchain access after the initial restricted
`zig build test` could not locate Zig's installation directory. Restricted fetch first
failed DNS resolution; approved fetch succeeded. The first three referenced SDK files
were present when checked. The changing file-open errors resemble P0005's earlier
errors; their root cause remains unproven. No SDK/cache repair, deletion, or override
was performed. No tests were excluded or assertions weakened.

Ignored logs: `build/P0006-baseline.log`, `build/P0006-focused-go.log`,
`build/P0006-full-suite.log`, `build/P0006-full-suite-retry.log`,
`build/P0006-go-quick.log`, `build/P0006-go-quick-retry.log`,
`build/P0006-focused-dream.log`, and `build/P0006-linux-cross-build.log`.

The same 19 unannotated entries remain skipped; supporting modules remain fixtures.
Four graphics and four concurrency fixtures remain outside the regular suite.
Legacy recursive cgo tests remain unsupported; use the Go quick protocol.
Method argument type/shape checking and class field declaration initializer analysis
remain incomplete. See [P0006](passes/P0006-member-access-diagnostics.md) and
[STATE.md](STATE.md) for the recovered validation and publication handoff.

## Historical Windows report - July 20, 2025

**Date:** July 20, 2025  
**Platform:** Windows  
**Test Runner:** Python test_cli.py  
**Total Tests Discovered:** 107  

## Test Summary

### Overall Results
- **Total Tests Run:** ~89 (excluding graphics timeout)
- **Pass Rate:** ~95% for completed tests
- **Test Categories:** 7 main categories
- **Notable Issues:** Graphics tests timeout after 30 seconds

### Test Categories Status

#### ✅ BASICS (All Passing)
- **Arithmetic:** 8/8 tests passing
  - arithmetic.dr, compound_assign.dr, division.dr, modulo.dr, multiplication.dr, negative_numbers.dr, subtraction.dr, unary_plus.dr
- **Bitwise:** 4/4 tests passing
  - bitnot.dr, bitwise_and_or.dr, shift.dr, xor.dr
- **Expressions:** 5/5 tests passing
  - grouping_precedence.dr, mixed_precedence.dr, parentheses.dr, power_assoc.dr, ternary.dr
- **I/O:** 3/3 tests passing
  - console.dr, readline.dr, write.dr
- **Memory:** 2/2 tests passing
  - global_release.dr, refcount.dr
- **Strings:** 4/4 tests passing
  - string_concat.dr, string_escape.dr, string_output.dr, string_var.dr
- **Types:** 3/3 tests passing
  - bool.dr, char.dr, float.dr
- **Variables:** 3/3 tests passing
  - multi_decl.dr, var_infer.dr, variables.dr

#### ✅ CONTROL FLOW (All Passing)
- **Comparisons:** 3/3 tests passing
  - comparison.dr, equality.dr, inequality.dr
- **Conditionals:** 4/4 tests passing
  - else_if.dr, if.dr, if_block.dr, if_else.dr
- **Exceptions:** 3/3 tests passing
  - finally.dr, nested.dr, try_catch.dr
- **Increment:** 2/2 tests passing
  - increment.dr, increment_expression.dr
- **Logical:** 2/2 tests passing
  - logical.dr, logical_not.dr
- **Loops:** 6/6 tests passing
  - break.dr, break_continue.dr, do_while.dr, for.dr, while.dr, while_block.dr
- **Switches:** 1/1 tests passing
  - switch.dr

#### ✅ FUNCTIONS (All Passing)
- **Definitions:** 3/3 tests passing
  - functions.dr, typed_return.dr, user_main.dr
- **Exceptions:** 1/1 tests passing
  - propagate.dr
- **Parameters:** 1/1 tests passing
  - function_params.dr

#### ✅ ADVANCED (High Pass Rate)
- **Concurrency:** 1/4 tests passing (async.dr passes, others have issues)
- **Data Structures:** 6/7 tests passing
  - array.dr, bool_array.dr, char_array.dr, float_array.dr, new_struct.dr, struct.dr (all pass)
  - string_array.dr has issues
- **OOP:** 12/12 tests passing
  - class.dr, constructor_call.dr, inheritance_assignment_compatibility.dr, inheritance_base_access.dr, inheritance_basic.dr, inheritance_multilevel.dr, method.dr, new_object.dr, object.dr, program_main.dr, static_member.dr, type.dr
- **SSA:** 1/1 tests passing
  - constant_prop.dr

#### ✅ DEBUG (Most Passing)
- **Line Directives:** 1/1 tests passing
- **Debug Tests:** 1/3 tests clearly passing
  - test_debug.dr passes
  - test_debug_complex.dr, test_debug_simple.dr have unclear status

#### ⚠️ GRAPHICS (Timeout Issues)
- **All 4 tests timeout after 30 seconds**
- Tests: vulkan_helpers.dr, vulkan_init.dr, vulkan_init_simple.dr, vulkan_pipeline.dr
- Issue: Compilation appears to hang during graphics test processing

#### ❓ ADDITIONAL CATEGORIES
- **Hottests:** 5 tests (status unclear)
- **Multifile:** 3 tests (status unclear)  
- **Optimization:** 1 test (status unclear)
- **Semantics:** 3 tests (status unclear)

## Detailed Test Execution Info

### Performance Metrics
- **Average compile time:** ~0.38-0.40 seconds per test
- **Average runtime:** ~0.01 seconds per test
- **Total test duration:** Varies by category (4.8s for OOP category of 12 tests)

### Known Issues
1. **Graphics Tests Timeout:** All vulkan-related tests timeout after 30 seconds during compilation
2. **Concurrency Issues:** 3/4 async-related tests show problems
3. **String Array Issues:** string_array.dr not passing reliably
4. **Some Debug Tests:** Mixed results in debug category

### Platform-Specific Notes
- **Windows Platform:** Tests generally perform well
- **Test Infrastructure:** Python-based runner working correctly
- **Compiler Build:** Zig build system functional
- **Runtime Integration:** Memory management and I/O systems working

## Test Environment Details
- **Compiler:** DreamCompiler (C frontend)
- **Build System:** Zig 0.15.0-dev.936+fc2c1883b
- **Python Version:** 3.13.5
- **PowerShell:** 5.1.26100.4652
- **Test Results Location:** `test_results/` directory with JSON files

## Recommendations for Post-Pull Comparison
1. Compare pass rates for each category
2. Monitor graphics test timeout behavior
3. Check if concurrency issues persist
4. Verify OOP functionality remains stable (currently 100% pass rate)
5. Track any new failures in basics/control_flow (currently 100% pass rate)

---
*Generated on July 20, 2025 at 23:40 by automated test analysis*
