export const repository = 'https://github.com/Ddemon26/DreamCompiler';
export const sourceBranch = 'codex/compiler-stabilization';
export const groups = [
  { name: 'Getting started', pages: [
    ['Overview', 'index.md'], ['Introduction', 'getting-started/intro.md'],
    ['Quick start', 'getting-started/quick-start.md'], ['Installation & build', 'development/compiler/build.md'],
  ] },
  { name: 'Language', pages: [
    ['Types & variables', 'fundamentals/variables.md'], ['Type system', 'fundamentals/type-system.md'],
    ['Control flow', 'language-features/language-guide/control-flow.md'], ['Functions', 'language-features/functions.md'],
    ['Operators', 'language-features/operators.md'], ['Modules', 'advanced-topics/modules.md'],
    ['String ownership', 'development/string-ownership.md'],
  ] },
  { name: 'Compiler & project', pages: [
    ['Compiler architecture', 'development/compiler/index.md'], ['Source map', 'development/source-map.md'],
    ['Implementation status', 'development/implementation-status.md'], ['Contributing', 'development/developer-guide.md'],
    ['Testing', 'development/compiler/testing.md'], ['Compiler dogfooding', 'development/compiler-dogfooding.md'],
  ] },
  { name: 'Reference & archives', pages: [
    ['Language reference', 'reference/language-reference.md'], ['Grammar', 'reference/grammar/Grammar.md'],
    ['Historical references', 'reference/README.md'], ['Changelog', 'reference/changelog.md'],
  ] },
];
export const pages = groups.flatMap(group => group.pages.map(([title, path]) => ({ title, path, group: group.name })));

export function docURL(path, hash = '', base = new URL('./', globalThis.location?.href || 'https://example.org/')) {
  const url = new URL(base);
  url.search = '';
  url.hash = '';
  url.searchParams.set('doc', path);
  url.hash = hash;
  return url;
}

export function validDocPath(path) {
  if (!path || /[\\%?#\u0000-\u001f]/.test(path)) return false;
  try {
    const url = new URL(path, 'https://docs.invalid/');
    return url.origin === 'https://docs.invalid' && decodeURIComponent(url.pathname) === '/' + path && path.endsWith('.md');
  } catch { return false; }
}

export function githubURL(path, hash = '') {
  const url = new URL(`${repository}/blob/${sourceBranch}/${path.split('/').map(encodeURIComponent).join('/')}`);
  url.hash = hash;
  return url;
}

// Resolve against repository docs paths, not the reader's query-string URL.
export function resolveContentLink(href, currentPath, base) {
  if (href.startsWith('#')) return docURL(currentPath, href, base);
  if (/^[a-z][a-z\d+.-]*:|^\/\//i.test(href)) return new URL(href, base);
  const repoURL = new URL(href, `https://repository.invalid/docs/${currentPath}`);
  const repoPath = decodeURIComponent(repoURL.pathname.slice(1));
  if (repoPath.startsWith('docs/')) {
    let path = repoPath.slice(5);
    if (path.endsWith('/')) {
      path = path === 'reference/grammar/' ? path + 'Grammar.md'
        : /^reference\/v1\.[01]\/$/.test(path) ? 'reference/README.md' : path + 'README.md';
    }
    if (validDocPath(path)) return docURL(path, repoURL.hash, base);
    return new URL(path + repoURL.search + repoURL.hash, base);
  }
  return githubURL(repoPath, repoURL.hash);
}

export function headingSlug(text) {
  return text.toLowerCase().trim().replace(/[^\p{L}\p{N}\s-]/gu, '').replace(/\s+/g, '-').replace(/-+/g, '-') || 'section';
}
