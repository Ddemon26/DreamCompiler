# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0025 - first production compiler dogfooding**.
- Record: [P0025](passes/P0025-compiler-dogfooding.md).
- Parent/fetched approved remote: `fe657146e26209db3936b52cbb6f413723d5558e` (P0024).
- Resolve the pass commit by its exact `Pass-ID: P0025` trailer.
- Publication requires push and matching `git ls-remote`; inspect native CI before
  selecting the next task.

The user selected a first Dream-written production component ahead of the next
T011 slice. This pass started clean in the approved local checkout/branch/origin,
one worktree and no competing active project chat in app inventory. Approved
fetch matched HEAD after restricted `.git/FETCH_HEAD` write denial. P0024
[CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43647310315) passed
native Linux/Windows checks and full suites for the exact parent SHA.

`src/driver/source_text.dr` now supplies production entry-file CRLF normalization.
It borrows writable NUL-terminated input and uses pointer cursors without allocation.
The C-only host `dreamc-bootstrap` retains the C seed, emits cached library C from
tracked Dream sources, and builds independently of generated code. The production
target compiler links the Dream implementation and runtime without the seed.
Host/target separation works in the Windows-to-Linux cross-build. Both executables
are installed. See the [contract](../docs/development/compiler-dogfooding.md).

`--emit-library-c OUTPUT SOURCE` uses real parser/semantic/common C emission with
external synchronous function linkage, forward prototypes and no main or host
compilation. Non-function top-level nodes, main and async are rejected; module/
object modes cannot be combined with it. Source rejection preserves existing
output; open/write failures fail. Program/object emission retains prior behavior.

The new Go/CI group checks 97,912 inputs against the C seed and independent expected
output, complete-buffer equivalence, boundaries and repeated calls. It checks
bootstrap/production helper C equality, separately linked forward/scalar/pointer
functions, rejection/output behavior and installed-compiler newline variants.
An incorrect initial idempotence assumption was corrected to preserve the seed's
single-pass CRCRLF behavior; independent byte and boundary checks remain.

Baseline and final full suite: **154 discovered, 136 passed, 18 skipped, zero
failures/errors**; final 46.69s. All **twenty-two Go quick groups passed**, 45.446s;
Python **five passed**, 0.058s. Focused group passed; final expanded checks passed
in quick. Linux compiler cross-build and harness cross-link passed; Linux binaries
were not run locally. Native P0025 runtime awaits CI. See
[MostRecentTestResults.md](MostRecentTestResults.md).

Same 18 skips/exclusions and prior ownership/shape/operator/allocation-failure
limits remain. Lexer, parser, semantics and emitter are still C. Helper C equality
does not prove successive whole-compiler rebuilds. Custom-type parameters, shared
module types, compiler data structures, ownership and host I/O remain future work.
Embedding hosts own runtime lifecycle for library functions needing it; the
normalizer needs none. Tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4.
Approved SDK/cache access was used without content/permission changes.

Next: verify P0025 publication/native CI and repair failures first. Otherwise
resume T011 conditional (`?:`) string initializer ownership with live-registry
evidence for literal/borrowed/managed/null branches and once-only branch effects
in ordinary/module entries. A future lexer migration needs its own token/storage
interface and equivalence pass. Do not start a second pass here.
