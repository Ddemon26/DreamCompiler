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

**P0007 publication and Linux/Windows CI verified. Verify P0008 publication before allocating P0009.**
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
