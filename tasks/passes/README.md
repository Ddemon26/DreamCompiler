# Pass checkpoints

| ID | Date | Result | Commit identity |
| --- | --- | --- | --- |
| [P0001](P0001-baseline.md) | 2026-11-03 | Reproducible compiler baseline | `bec119b` (legacy, no trailer) |
| [P0002](P0002-zig-inheritance.md) | 2026-11-03 | Zig 0.17.1 and declaration-based inheritance | `e52280a` (legacy, no trailer) |
| [P0003](P0003-multi-file.md) | 2026-11-03 | Function modules, dependency and export checks | `6fe29c4`, `Pass-ID: P0003` |
| [P0004](P0004-daily-agent-workflow.md) | 2026-11-03 | Daily-agent contract, handoff, prompt, and delivery gate | `Pass-ID: P0004` |
| [P0005](P0005-primitive-return-checks.md) | 2026-11-04 | Primitive return checks, recovered checkout, local-only workflow | `Pass-ID: P0005` |
| [P0006](P0006-member-access-diagnostics.md) | 2026-11-06 | Member receiver/access diagnostics; recovered passing validation | `Pass-ID: P0006` |
| [P0007](P0007-loop-optimization-stability.md) | 2026-11-06 | Optimized loop compilation no longer crashes/stalls; skipped fixture now checked | `Pass-ID: P0007` |
| [P0008](P0008-optimization-emission-design.md) | 2026-11-07 | Source-audited literal-arithmetic IR-to-AST design; implementation remains open | `Pass-ID: P0008` |
| [P0009](P0009-literal-emission.md) | 2026-11-08 | Checked literal SCCP results reach C/object emission; fallback and internal failures verified | `Pass-ID: P0009` |
| [P0010](P0010-method-arguments.md) | 2026-11-10 | Primitive scalar method checks; recovered passing full/quick validation | `Pass-ID: P0010` |
| [P0011](P0011-constructor-arguments.md) | 2026-11-11 | Exact primitive constructor types/arity and artifact-free rejection; final full/quick validation passed | `Pass-ID: P0011` |
| [P0012](P0012-parser-parameter-recovery.md) | 2026-11-12 | Malformed parameter parsing terminates; 75 bounded ordinary/module/dependency rejections and full/quick checks passed | `Pass-ID: P0012` |
| [P0013](P0013-pointer-parameter-emission.md) | 2026-11-12 | Primitive pointer parameter signatures preserved; generated-C and exact runtime checks, full/quick checks passed | `Pass-ID: P0013` |
| [P0014](P0014-indexed-string-ownership.md) | 2026-11-12 | Indexed scalar strings own managed copies; checked lifetime and exact output in both modes, full/quick checks passed | `Pass-ID: P0014` |
| [P0015](P0015-linux-string-harness.md) | 2026-11-13 | Linux ownership harness flags match runtime; both modes cross-link on Windows and full/quick checks pass | `Pass-ID: P0015` |
| [P0016](P0016-indexed-console.md) | 2026-11-13 | Indexed primitives preserve console formats; string elements/characters distinguished, both modes and full/quick checks pass | `Pass-ID: P0016` |
| [P0017](P0017-literal-string-ownership.md) | 2026-11-13 | Scalar literal string declarations own managed copies; checked cleanup and exact output in both modes, full/quick checks pass | `Pass-ID: P0017` |
| [P0018](P0018-identifier-string-ownership.md) | 2026-11-13 | Scalar identifier string declarations own independent copies; nested source lifetime and both modes/full/quick checks pass | `Pass-ID: P0018` |
| [P0019](P0019-member-string-ownership.md) | 2026-11-13 | Scalar member string declarations own independent copies; inherited/static/temporary receivers and both modes/full/quick checks pass | `Pass-ID: P0019` |
| [P0020](P0020-for-header-string-ownership.md) | 2026-11-14 | For-header strings use scoped declarations and loop-exit cleanup; both modes/full/quick checks pass | `Pass-ID: P0020` |
| [P0021](P0021-call-string-ownership.md) | 2026-11-14 | Scalar string call initializers own copies without result transfer; both modes/full/quick checks pass | `Pass-ID: P0021` |
| [P0022](P0022-assigned-string-ownership.md) | 2026-11-14 | Owned scalar replacements copy before old-value release; self/nested assignment, null defaults, both modes/full/quick checks pass | `Pass-ID: P0022` |
| [P0023](P0023-borrowed-string-containers.md) | 2026-11-14 | Borrowed string arrays/pointers avoid scalar cleanup; four checked modes preserve owned releases, full/quick checks pass | `Pass-ID: P0023` |
| [P0024](P0024-loop-exit-strings.md) | 2026-11-14 | Break/continue release exited owned scalar strings; target/header boundaries, both checked modes and full/quick checks pass | `Pass-ID: P0024` |
| [P0025](P0025-compiler-dogfooding.md) | 2026-11-14 | Production compiler uses Dream source normalizer; host seed, function-library emission, differential bytes and full/quick/cross checks pass | `Pass-ID: P0025` |
| [P0026](P0026-repository-cleanup.md) | 2026-11-14 | Eleven obsolete files removed; operational docs consolidated; source hashes/discovery unchanged and full/quick checks pass | `Pass-ID: P0026` |
| [P0027](P0027-go-tooling-cleanup.md) | 2026-11-14 | Obsolete Go builders/modes retired; quick selectors preserved, failure probes and full/quick checks pass | `Pass-ID: P0027` |
| [P0028](P0028-docs-website.md) | 2026-11-14 | Approved square-edged homepage/reader; 21 routes, 8 checks, browser QA and exact-output examples; live main:/docs release awaits owner instruction | `Pass-ID: P0028` |
| [P0029](P0029-docs-release-verification.md) | 2026-11-15 | Existing Pages release verified: merged PR, passing native CI/deployment, 27 exact live responses and 8 Node checks; T011 unblocked | `Pass-ID: P0029` |
| [P0030](P0030-conditional-string-ownership.md) | 2026-11-14 | Conditional scalar strings own selected-value copies; both checked modes and full/23-group quick/cross checks pass; compiler handoff restored | `Pass-ID: P0030` |
| [P0031](P0031-loop-exit-classes.md) | 2026-11-15 | Class locals released before break/continue; both checked modes, 138 Dream passes and 24 quick groups; cross-build passed | `Pass-ID: P0031` |
| [P0032](P0032-assignment-initializer-ownership.md) | 2026-11-15 | Plain assignment scalar string initializers own independent copies; both checked modes, 139 Dream passes and 25 quick groups; cross-build passed | `Pass-ID: P0032` |
| [P0033](P0033-owned-string-append.md) | 2026-11-15 | Owned scalar string append replaces old storage safely; both checked modes, ten rejection cases, 142 Dream passes and 26 quick groups; cross-build passed | `Pass-ID: P0033` |
| [P0034](P0034-void-return-strings.md) | 2026-11-15 | Owned scalar strings released before bare synchronous void return; both checked modes, 143 Dream passes and 27 quick groups; cross-build passed | `Pass-ID: P0034` |
| [P0035](P0035-void-return-classes.md) | 2026-11-15 | Scalar class locals released before bare synchronous void return; both checked modes, 144 Dream passes, 28 quick groups and Linux cross-build passed | `Pass-ID: P0035` |
| [P0036](P0036-scalar-return-cleanup.md) | 2026-11-15 | Primitive scalar returns capture results before scoped string/class cleanup; both checked modes, 145 Dream passes, 29 quick groups and Linux cross-build passed | `Pass-ID: P0036` |
| [P0037](P0037-local-string-return.md) | 2026-11-15 | Direct owned-local string results survive reverse-order cleanup of other locals; both modes, 146 Dream passes, 30 quick groups and Linux cross-build passed | `Pass-ID: P0037` |
| [P0038](P0038-literal-string-return.md) | 2026-11-15 | Direct literal string results survive full scoped cleanup; both modes, 147 Dream passes, 31 quick groups and Linux cross-build passed | `Pass-ID: P0038` |
| [P0039](P0039-conditional-literal-return.md) | 2026-11-15 | Literal-only conditional string results capture once before scoped cleanup; both modes, 148 Dream passes, 32 quick groups and Linux cross-build passed | `Pass-ID: P0039` |
| [P0040](P0040-conditional-local-return.md) | 2026-11-15 | Conditional literal/owned-local results preserve selected storage during reverse cleanup; both-mode lifetime and full-suite checks passed | `Pass-ID: P0040` |
| [P0041](P0041-borrowed-parameter-return.md) | 2026-11-15 | Direct unchanged borrowed string-parameter returns release scoped locals; both-mode lifetime, 24 rebound controls and full/quick/cross checks passed | `Pass-ID: P0041` |
| [P0042](P0042-conditional-borrowed-return.md) | 2026-11-15 | Conditional unchanged borrowed scalar parameter results release other locals and preserve selected owned storage; both-mode lifetime and full-suite checks passed | `Pass-ID: P0042` |
| [P0043](P0043-member-return-audit.md) | 2026-11-16 | Source-audited member backing and alias/call barriers; read-only borrowed class-member contract and acceptance matrix; implementation remains open | `Pass-ID: P0043` |
| [P0044](P0044-borrowed-member-return.md) | 2026-11-16 | Direct read-only instance string-field results capture before cleanup; both-mode lifetime, 196 fallback controls and full-suite checks pass | `Pass-ID: P0044` |
| [P0045](P0045-conditional-member-return.md) | 2026-11-16 | Conditional read-only instance-field results preserve selected owned storage and clean other locals; both-mode lifetime, fallback and full-suite checks pass | `Pass-ID: P0045` |
| [P0046](P0046-static-member-return-audit.md) | 2026-11-16 | Source-audited static slot/backing lifetime; direct type-qualified class-field contract and both-mode acceptance; implementation remains open | `Pass-ID: P0046` |
| [P0047](P0047-static-member-return.md) | 2026-11-16 | Direct read-only static class string-field results capture once before reverse local cleanup; both-mode lifetime, exact C and guarded fallback checks pass | `Pass-ID: P0047` |
| [P0048](P0048-conditional-static-return.md) | 2026-11-16 | Conditional proven static string-field leaves mix with borrowed/owned/literal results, preserve selected owned storage and clean other locals; both-mode lifetime and guarded fallback checks pass | `Pass-ID: P0048` |
| [P0049](P0049-owned-call-result-audit.md) | 2026-11-17 | Reproduced owned call-result accumulation with eight checked probes; defined conservative free-function initializer adoption contract; compiler unchanged | `Pass-ID: P0049` |

**Verify P0049 publication/native CI, then implement proven-owned free-function call initializer adoption in T011.**
Verify current git history and delivery before allocating another ID.
Allocate monotonically, never reuse a committed ID, and record a blocked attempt
without presenting it as a delivered pass. A run that only reports a blocker need not
make an empty commit. Recover a documented unpublished pass before allocating new work.

Use [TEMPLATE.md](TEMPLATE.md). Records contain a parent SHA, test evidence, limits,
and next steps. Find a record's own commit through its exact trailer:

```sh
git log --all --grep='^Pass-ID: P0004$' --format='%h %s' -1
```

A commit cannot contain its own final SHA. The git history and final remote-ref
verification supply that identity. Do not amend or rewrite old checkpoints to add IDs.
Historical records describe their measured state; they are not a current test run.
