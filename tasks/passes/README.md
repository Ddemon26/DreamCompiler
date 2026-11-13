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

**P0015 validation passed. Verify publication and inspect native CI before allocating P0016.**
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
