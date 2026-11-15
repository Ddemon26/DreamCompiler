# P0028 - Square-edged homepage and documentation reader

- Date: 2026-11-14
- Task: explicit user website redesign request and selected mockups
- Parent checkpoint: `4f1c24ac6c5e88892666ed0e87066a0ecf01c430`
- Commit lookup: exact `Pass-ID: P0028` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

The owner selected the Source first introduction and Guide page documentation
layout, using Workbench's charcoal/lavender palette, then required square edges.
The previous website was a nested-navigation Markdown viewer with stale path
resolution and version claims. The approved designs now form a static homepage
and documentation reader in `docs/index.html`, `index.css`, `site.js` and
`site-data.js`. There are no rounded corners, pills, or decorative shadows.
The obsolete split light/dark/base styles are replaced by shared theme tokens.

The homepage has two keyboard-operable examples, clear documentation entry points,
the compilation path, and source-build guidance. Example output is labeled as
illustrative; there is no browser compiler. The reader provides 21 real sidebar
targets, topic filtering, breadcrumbs, article heading links, code copying,
previous/next links, source links, history navigation, loading/error/retry states,
mobile navigation, and a persistent optional light theme. Query routes work under
the repository Pages subpath without server rewrites. Relative Markdown/image
paths resolve from the source document; repository evidence outside `docs/`
links to the approved integration branch. Historical version material is labeled.

New quick-start and implementation-status guides avoid unqualified release or
performance claims. The docs overview, introduction and source map link the new
entry points. `docs/development/docs-site.md` records maintenance and the owner's
square-edge styling requirement. Local Marked 18.2.0 and DOMPurify 3.4.18 browser
modules were extracted from integrity-verified official npm tarballs with licenses
and a durable integrity record. The site needs no build, installation, or CDN.
Node is needed only for the optional website checks.

## Validation

Started in the clean primary checkout on the attached approved branch with the
expected origin, one worktree and no other active project chat. Fetch succeeded
and matched HEAD. P0027 publication matches the fetched remote. Both native jobs
in [P0027 CI 43654089255](https://github.com/Ddemon26/DreamCompiler/actions/runs/43654089255)
completed successfully before this pass edited website files.

- `node --check docs/site.js`: **passed**.
- `node --test docs/site.test.mjs`: **8 passed, 0 failed/skipped**. Checks cover all
  unique existing sidebar paths, repository-subpath/query/heading routing,
  relative source-directory links, repository evidence destinations, directory
  references, invalid fetch paths including traversal, Unicode slugs, and square
  corners/no decorative shadows.
- Browser qualification via the local HTTP server: **all 21 sidebar pages loaded**
  through the actual local Marked/sanitizer paths with expected headings and no
  article errors. Homepage click and keyboard example selection, topic filtering
  including zero matches, internal link routing, browser back, direct heading
  links/reload, clipboard success, saved light-theme reload and dark restoration,
  archive notice, and HTTP 404/retry/navigation recovery **passed**.
- Mobile checks at **320px**: homepage and table-containing article had **no page
  horizontal overflow**. Menu open/select/close and table containment passed.
  Homepage at **768px** also had no page overflow. Desktop **1280px** homepage and
  three-column article were visually reviewed; temporary viewport overrides reset.
- A temporary real Markdown QA page verified removal of scripts, event handlers
  and unsafe link schemes, no injected document attribute, table wrapping, and
  distinct duplicate heading IDs. It was removed before commit. No fake route or
  test fixture is published.
- Windows Python initially served `.mjs` as `text/plain`, preventing all UI scripts
  from executing. Browser QA caught this; runtime modules now use `.js`, and the
  DOMPurify browser file is renamed without changing its bytes. Reloaded browser
  interactions and all subsequent checks passed.
- `zig version`: **0.17.1**. Both actual homepage snippets were extracted from HTML
  into ignored `build/P0028-example-{0,1}.dr` files. Sequential
  `zig build run -- build/P0028-example-0.dr` / `-1.dr`, then `.\dream.exe`,
  **passed with exact output `Hello, Dream!` and `8`**, exit zero.
  Initial direct compiler invocations failed while launching their default GCC;
  using the documented Zig build entry point configured `CC=zig cc` and passed.
- **408 protected file hashes unchanged** across compiler/runtime sources, fixtures,
  runner/Go tooling, plugins, build graph and CI. No compiler assertions, inputs,
  discovery rules, or exclusions changed.
- **48 local links** in new/revised guides resolved before pass metadata; final
  record/handoff/index links and `git diff --check` are checked before commit.
- Full compiler/Go/runner suites and cross-build **not rerun**, because no related
  inputs changed. Reused compiler evidence: **P0027, November 14, 2026 UTC**, 136
  Dream passes/18 skips/no failures, 22 quick groups, 5 runner tests, plus the exact
  parent's successful native Linux/Windows CI above. This website pass does not
  qualify new compiler behavior or remove any skips.

Local server and toolchain/network operations used normal approval controls.
Ignored evidence: `build/P0028-*` (protected hashes, npm integrity metadata/tarballs,
example sources). Homepage/article screenshots are conversation-owned artifacts.

## Limits and handoff

The integration pass requires commit, push and matching remote SHA. Resolve that
commit through its trailer; do not put this record's own SHA here. GitHub Pages
was read through `gh api repos/Ddemon26/DreamCompiler/pages`: **built, legacy
source `main:/docs`**. The approved branch push does **not** publish these changes
to the live website. Do not merge into `main` or change Pages settings without a
new explicit owner instruction. The implementation is concrete and reviewable;
live release is the next step needing that instruction.

The topic search is title/group filtering, not full-text search. Legacy tutorial,
API and grammar accuracy and all their body links remain a separate audit; only
new/changed guides and sidebar navigation are qualified here. Existing compiler
ownership/shape/unwinding limits and 18 skips remain. Browser forward, disabled
JavaScript/storage/clipboard-permission environments and native Linux browser
execution were not separately exercised. Raw Markdown/GitHub fallback links and
clipboard selection fallback are provided without claiming those environments
were tested. No Pages deployment or Sites-hosted copy is claimed.

Next: obtain explicit publication instruction for `main:/docs`, publish the
reviewed pass through the authorized procedure, and check the live homepage/docs.
After release and CI verification, resume the bounded T011 task unless reprioritized.
