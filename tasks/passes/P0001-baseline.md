# P0001 - Reproducible compiler baseline

- Date: 2026-11-03
- Commit: `bec119b` (recorded retrospectively; this commit predates Pass-ID trailers)
- Parent: `eb69850`

Updated the build to Zig 0.15.1, stopped compilation on parser/semantic errors,
replaced moving AST arena storage with aligned stable blocks, corrected expression
types, and made test reporting handle expected errors, empty output, runtime exits,
and timeouts. Added the initial Go driver and Python runner regressions.

Measured evidence: 117 Dream entries; 95 passed, 22 skipped, zero failures/errors;
three Go stability groups; four runner tests; Linux cross-build. Linux binaries were
not executed locally. This is historical evidence, not the current baseline.

The next pass upgraded Zig and removed class-name-specific inheritance handling.
