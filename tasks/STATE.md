# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0028 - square-edged homepage and documentation reader**.
- Record: [P0028](passes/P0028-docs-website.md).
- Parent/fetched approved remote: `4f1c24ac6c5e88892666ed0e87066a0ecf01c430` (P0027).
- Resolve the pass commit by its exact `Pass-ID: P0028` trailer.
- Integration publication requires push and matching `git ls-remote`.
- **Live Pages release requires new owner instruction: Pages still serves `main:/docs`.**

The owner approved the Source first homepage and Guide page documentation layout,
with Workbench's dark/lavender styling, then requested square edges throughout.
P0028 implements those choices with no rounded controls/pills or decorative
shadows. The same structure supports an optional light theme. The static site
has no installation/build/CDN requirement. Local Marked and DOMPurify modules
are pinned, integrity checked and licensed.

The working reader has 21 actual sidebar pages, title/group topic filtering,
source-relative Markdown links, heading outlines/deep links, code copying,
previous/next/source navigation, history handling, loading/error/retry states,
mobile menus and theme persistence. New quick-start and implementation-status
guides give bounded examples and evidence links; existing tutorial/API accuracy
remains separate. See [website maintenance and design rules](../docs/development/docs-site.md).

Started clean in the approved checkout/branch/origin, one worktree and no competing
active writer. Fetch matched HEAD; both native P0027
[CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/43654089255) passed.
Website checks: **8 Node checks passed**, all 21 pages rendered in the browser,
search/back/deep-link/copy/themes/archive/404 recovery passed. A real temporary
Markdown fixture proved sanitization, duplicate IDs and table rendering; removed
before commit. Mobile 320px and tablet 768px overflow checks passed; desktop
1280px homepage/article reviewed. Windows `.mjs` MIME behavior was caught and
fixed by using `.js` runtime modules. Both homepage examples compiled with
documented `zig build run` and printed exact output using Zig 0.17.1.

**408 protected compiler/test/plugin/build/CI hashes unchanged.** Full compiler,
Go, runner and cross-build checks were not rerun. Reused **P0027 evidence dated
November 14, 2026 UTC**: 136 Dream passes/18 skips/no failures, 22 quick groups,
5 runner tests, plus successful native CI above. See
[results](MostRecentTestResults.md) for the website checks and limits.

GitHub Pages metadata is **built**, **legacy**, **`main:/docs`**. Pushing this
integration pass does not update the live site. Root instructions prohibit a
`main` merge or changed publishing destination without a new user instruction.
The next concrete step is owner authorization for the live release, then live
homepage/docs checks. Do not silently merge/reconfigure Pages or create another
hosted copy. Existing compiler limits and the T011 conditional-string initializer
task remain; do not begin a different feature while this delivery is unresolved.
