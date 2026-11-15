# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0029 - verify the deployed documentation release**.
- Record: [P0029](passes/P0029-docs-release-verification.md).
- Parent/fetched approved remote: `9edc4b9ba0eb636e091ba9a3df9d4e661646d2e5` (P0028).
- Resolve the pass commit by its exact `Pass-ID: P0029` trailer.
- Integration publication requires push and matching `git ls-remote`.
- **P0028's live release gate is resolved. T011 is the next compiler task.**

P0028 implemented the approved square-edged homepage and documentation reader.
Since its handoff, [PR #231](https://github.com/Ddemon26/DreamCompiler/pull/231)
was merged into `main` outside this run, at
`6b34823f641bdb4585d7f415b6a08acd0074a791` on November 15, 2026 UTC.
The [Pages build and deployment](https://github.com/Ddemon26/DreamCompiler/actions/runs/43660436390)
succeeded. Pages remains **built, legacy, `main:/docs`**; no merge, Pages-setting
change or new hosting destination was performed by P0029.

The [live homepage](https://ddemon26.github.io/DreamCompiler/) and all 21 sidebar
Markdown documents were checked over HTTPS. **27 responses** (homepage, stylesheet,
four JavaScript modules and 21 documents) returned HTTP 200 and matched P0028's
Git blobs byte for byte. Module/CSS MIME types and the quick-start query/deep-link
shell passed. The merge's `docs/` tree also matches P0028. All **8 Node checks**
and JavaScript syntax passed. Browser inventory was empty, so this run does not
claim deployed rendering, clicks, clipboard or viewport checks; P0028's local
browser evidence remains dated and separate. See [results](MostRecentTestResults.md).

P0028 [integration CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43658070457)
and [merged-main CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43660437950)
both passed native Linux and Windows jobs, including build, runner, quick and
Dream suite steps. No failed baseline was found. No compiler/runtime/test/build
inputs changed in P0029. Local compiler suites and cross-build were **not rerun**;
reuse **P0027 / November 14, 2026 UTC**: 136 Dream passes, 18 skips, no failures,
22 Go quick groups and 5 runner tests. This pass qualifies release delivery,
not additional compiler behavior or legacy tutorial/API accuracy.

Started clean in the approved checkout/attached branch/origin, one worktree and
no competing active project writer. Fetch and `git ls-remote` confirmed P0028
was already published. The schedule already uses the local project directory.
P0029 records release evidence and updates the handoff; it does not republish
website assets to `main`.

Next invocation: verify P0029 integration publication and native CI, repairing
failures first. Otherwise resume T011 scalar conditional (`?:`) string initializer
ownership. Reproduce literal/borrowed/managed/null branches with the checked live
registry, then qualify independent selected-value storage and once-only branch
evaluation in ordinary/module-entry modes. Return ownership, class-instance jump
cleanup and exception/finally unwinding remain separate. Existing 18 skips and
shape/operator/ownership limits remain.
