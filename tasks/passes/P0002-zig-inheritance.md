# P0002 - Zig 0.17.1 and declaration-based inheritance

- Date: 2026-11-03
- Commit: `e52280a` (recorded retrospectively; this commit predates Pass-ID trailers)
- Parent: `bec119b`

Installed the verified Zig 0.17.1 release in the user's Programs directory and
migrated the build's I/O, module, formatting, and argument APIs. Resolved class
hierarchies from declarations, preserved base subobjects and typed fields, supported
base calls/hidden fields/static dispatch/upcasts, and rejected invalid hierarchies.

Measured evidence: 129 Dream entries; 107 passed, 22 skipped, zero failures/errors;
16 focused inheritance cases; three Go stability groups; four runner tests; Linux
cross-build. A transient runtime-cache read failure was isolated and the full suite
subsequently passed. Linux binaries were not executed locally.

Virtual dispatch, access control, and implicit base-constructor calls remain unsupported.
The next pass implemented function-only multi-file compilation.
