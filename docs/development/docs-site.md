# Documentation website

The site is plain HTML, CSS, JavaScript modules, and Markdown. It needs no Node
installation or build step to deploy. `docs/index.html` contains the Source first
homepage and Guide page documentation shell selected by the project owner.

## Design rules

- Use the Workbench dark palette: charcoal surfaces, lavender accents, crisp rules.
- Use square corners throughout. Avoid rounded controls, pills, soft panels, and
  decorative shadows. Keep keyboard focus indicators clearly visible.
- Keep article line lengths readable and collapse navigation for small screens.
- Preserve the same structure in the optional light theme.
- Describe Dream as experimental. Distinguish verified behavior from historical
  or planned features; do not invent performance or platform claims.

## Preview locally

From the repository root:

```sh
python -m http.server 8768 --bind 127.0.0.1 --directory docs
```

Open `http://127.0.0.1:8768/`. ES modules and Markdown loading require HTTP;
opening `index.html` directly as a local file is not supported.

For the offline navigation and design checks, run `node --test docs/site.test.mjs`
from the repository root. Node is only needed for these checks; no packages need
to be installed, and it is not needed to serve or deploy the site.

The homepage works without Markdown loading. Without JavaScript, the document
reader exposes links to raw Markdown and GitHub.

## Routes and content

- Home: `./`
- Documentation overview: `?doc=index.md`
- Quick start: `?doc=getting-started/quick-start.md`
- A section: `?doc=getting-started/quick-start.md#compile-and-run`

Query-based URLs work beneath the GitHub Pages repository subpath without server
rewrite rules. Browser back/forward and direct document links use the same route.
Relative Markdown links resolve from the source document's directory. Links
outside `docs/` open the integration branch on GitHub. Images use their source
directory too. Unknown or missing pages show a retry and navigation remains usable.

`site-data.js` lists sidebar groups and page paths. Add a page there to include it
in topic search and previous/next navigation. Search filters topic titles and groups;
it does not claim full-text content search. The article outline uses level-two
headings, with unique IDs generated for all heading levels.

`site.js` loads the local Marked parser and DOMPurify sanitizer only when a document
is opened. See [vendored dependencies](../vendor/README.md) for pinned versions,
licenses, integrity, and update instructions.

The homepage example output is illustrative text, not an in-browser compiler.
The quick-start guide contains the same executable examples.

## Publication

The existing Pages configuration was last reported as `main:/docs`. Publishing
the pass to `codex/compiler-stabilization` does not change that live website.
Confirm the current Pages source before a live release. The project agreement
requires a new user instruction before merging into `main` or changing publication
destinations. Do not silently reconfigure Pages or create another hosted site.

Check all sidebar targets, relative links, anchors, themes, loading failures, mobile
navigation, keyboard controls, and code copying before publishing website changes.
Legacy tutorial/API accuracy remains a separate audit.
