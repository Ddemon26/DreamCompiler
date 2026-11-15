# Browser dependencies

The documentation reader uses local, pinned browser modules. No CDN is contacted
when the reader runs. These files were extracted from official npm tarballs after
checking each tarball's SHA-512 against the registry's integrity field.

| Package | Version | Browser file | License |
| --- | --- | --- | --- |
| [Marked](https://github.com/markedjs/marked) | 18.2.0 | `marked.esm.js` | [MIT](marked-LICENSE) |
| [DOMPurify](https://github.com/cure53/DOMPurify) | 3.4.18 | `purify.es.js` | [MPL-2.0 or Apache-2.0](dompurify-LICENSE) |

Marked parses Markdown; DOMPurify sanitizes its HTML before insertion into the
article. Keep both steps. Code highlighting constructs text nodes and spans.
The DOMPurify module is renamed from `purify.es.mjs` to `purify.es.js`, preserving
its bytes, so common static servers serve it with a JavaScript MIME type.

## Integrity record

Pinned npm tarballs:

- `https://registry.npmjs.org/marked/-/marked-18.2.0.tgz`
  — `sha512-KaeL+tAbtvI930q7HE2PN5lYXYdT8EiSmxHlN6F3rsfb57wd5mp5W5TLazpdj4inTQBJdgjYwbs40W/0VUGU+Q==`
- `https://registry.npmjs.org/dompurify/-/dompurify-3.4.18.tgz`
  — `sha512-ofe3wI/uNm68HInABYFJYqQxRw1EaKAjNdrFVKqjUY+UZurvNd2L/HuQe097s/4RivWiZyP6tT7aoT8atjfhwg==`

Extracted browser-module SHA-256 values:

```text
marked.esm.js  162194ea902a9d6eca6011bb42b5732991ca650e017d0375b76f4204810c6c14
purify.es.js   1a06b721ad8b46d6174b21f22ea70290aaf3e5a81d65af3b7178113953f7c9b1
```

For updates, obtain the pinned package metadata and tarball from the official npm
registry. Verify its `dist.integrity`, extract only the browser module and license,
and update this record. Do not run package installation scripts. Retest Markdown
rendering, links, tables, code, sanitization, and loading failures after updating.
