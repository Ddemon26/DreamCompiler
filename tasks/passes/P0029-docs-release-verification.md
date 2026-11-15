# P0029 - Verify the deployed documentation release

- Date: 2026-11-15 UTC
- Task: T014 remaining live-release acceptance and prior-pass recovery
- Parent checkpoint: `9edc4b9ba0eb636e091ba9a3df9d4e661646d2e5`
- Commit lookup: exact `Pass-ID: P0029` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

P0028 left a concrete website and a live-release gate because Pages served
`main:/docs`. Before selecting another compiler feature, this run checked the
existing release. PR #231 had already merged P0028 into `main` outside this run.
The deployed site now serves the tested assets. T014's publication gate is closed
and the handoff can resume T011 after this pass's publication/CI verification.

No website implementation, compiler, test, build or CI inputs changed. Updated
state, backlog, results and pass index distinguish observed deployment from
P0028's historical pending instruction. The website maintenance guide now explains
post-deployment byte/MIME verification and the limit of HTTP-only evidence.
This run did not merge into `main`, change Pages settings or create another host.

## Validation

- Approved checkout `C:/Users/damon/cApps/DreamCompiler`, attached
  `codex/compiler-stabilization`, expected origin, clean worktree, one registered
  worktree and no other active project writer. The preceding scheduled memory
  recorded delivered P0024; intervening P0025-P0028 commits and P0028's manual
  chat handoff were inspected rather than repeating that work. The schedule
  already has local execution and the approved project directory; unchanged.
- Initial restricted fetch failed DNS resolution. Retrying through normal
  approval controls succeeded. Fetched branch and `git ls-remote` both matched
  parent P0028. No unpublished integration commit or divergence was found.
- GitHub PR metadata: [#231](https://github.com/Ddemon26/DreamCompiler/pull/231)
  **merged**, November 15, 2026 00:53:35 UTC, merge commit
  `6b34823f641bdb4585d7f415b6a08acd0074a791`.
  `git diff HEAD origin/main -- docs` was empty before edits.
- `gh api repos/Ddemon26/DreamCompiler/pages`: **built**, **legacy**,
  source **`main:/docs`**, URL `https://ddemon26.github.io/DreamCompiler/`.
  [Pages run 43660436390](https://github.com/Ddemon26/DreamCompiler/actions/runs/43660436390):
  **build, report-build-status and deploy passed** for the merge SHA.
- [P0028 CI 43658070457](https://github.com/Ddemon26/DreamCompiler/actions/runs/43658070457)
  and [merged-main CI 43660437950](https://github.com/Ddemon26/DreamCompiler/actions/runs/43660437950):
  **native Linux and Windows jobs passed**, including compiler build, Python
  runner, Go quick regressions and Dream suite steps. Additional P0028 CI
  run 43658735224 was also reported successful. Platform-inapplicable setup/
  cross-build steps remain skipped as designed; these are not Dream fixture skips.
- `node --check docs/site.js`: **passed**.
- `node --test docs/site.test.mjs`: **8 passed, 0 failed/skipped**, 118.474 ms.
- HTTPS probe against the published Pages subpath: **27 HTTP 200 responses**
  matched the exact P0028 Git blob bytes, covering the homepage, stylesheet,
  `site.js`, `site-data.js`, both vendored modules and **all 21 sidebar Markdown
  documents**. JavaScript and CSS MIME assertions passed. The query route
  `?doc=getting-started/quick-start.md#compile-and-run` served the exact homepage
  shell with HTTP 200. Checked at **2026-11-15 01:06:30 UTC**. Responses were
  compared to committed bytes rather than CRLF-transformed checkout files.
- Browser automation could not open `iab`; browser/app inventory was empty.
  **No live browser interaction or rendering checks were performed**. HTTP
  responses and deployment job success do not independently prove those checks.
  P0028's local 21-page/browser/mobile qualification applies to the identical
  published implementation, with its original limits, and was not rerun.
- Local compiler/Go/runner suites and cross-build **not rerun** for this
  documentation/evidence-only pass. Reused **P0027 / November 14, 2026 UTC**:
  **136 Dream passes, 18 skips, zero failures/errors**, **22 quick groups** and
  **5 runner tests**. Exact P0028 and merge native CI above provide newer
  platform evidence; no new local compiler counts are claimed.
- **93 local Markdown links resolved** across all six revised/new documents;
  referenced local entry points and probe/evidence paths were checked.
  **408 protected tracked compiler/test/plugin/build/CI inputs unchanged**
  against HEAD. Change-scope, unused pass-trailer, full diff review and
  `git diff --check` **passed** before commit.

Ignored probe/evidence: `build/P0029-verify-live.py`, `build/P0029-live-results.json`.
Network reads used normal approval controls without permission or cache changes.

## Limits and handoff

This pass records a release already performed outside the run; it grants no
permission for future `main` merges or changed publishing destinations. Its own
documentation changes are published only to the approved integration branch.
P0028's historical record remains intact. No new website build or deployment is
claimed for this pass. Legacy tutorial/API correctness, all body-link accuracy,
deployed interactive behavior and disabled-browser-feature environments remain
outside this run's checks. All prior compiler limits and 18 skips remain.

Verify this pass's commit, push and matching remote SHA, then native CI. If CI
passes, the next bounded task is T011 conditional scalar string initializers:
checked literal/borrowed/managed/null branches, independent selected-value
storage and once-only branch effects in ordinary/module-entry modes. Keep return
transfer and exception/finally unwinding separate. No T011 implementation was
started in this release-verification pass.
