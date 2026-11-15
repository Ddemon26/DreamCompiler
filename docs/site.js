import { groups, pages, sourceBranch, docURL, validDocPath, resolveContentLink, githubURL, headingSlug } from './site-data.js';

const base = new URL('./', location.href);
const byId = id => document.getElementById(id);
const article = byId('article');
const status = byId('article-status');
let activePath = '';
let pendingRequest;
let outlineObserver;
let renderer;

function element(tag, text, className) {
  const node = document.createElement(tag);
  if (text !== undefined) node.textContent = text;
  if (className) node.className = className;
  return node;
}

function buildNavigation() {
  for (const group of groups) {
    const section = element('section', undefined, 'topic-group');
    section.append(element('h2', group.name));
    for (const [title, path] of group.pages) {
      const link = element('a', title, 'topic-link');
      link.href = docURL(path, '', base);
      link.dataset.path = path;
      section.append(link);
    }
    byId('topic-nav').append(section);
  }
}

function setTheme(theme) {
  document.documentElement.dataset.theme = theme;
  byId('theme-toggle').textContent = theme === 'dark' ? 'Light theme' : 'Dark theme';
  byId('theme-toggle').setAttribute('aria-label', `Switch to ${theme === 'dark' ? 'light' : 'dark'} theme`);
  document.querySelector('meta[name="theme-color"]').content = theme === 'dark' ? '#10141c' : '#fafafd';
}
try { setTheme(localStorage.getItem('dream-site-theme') === 'light' ? 'light' : 'dark'); }
catch { setTheme('dark'); }
byId('theme-toggle').addEventListener('click', () => {
  const theme = document.documentElement.dataset.theme === 'dark' ? 'light' : 'dark';
  setTheme(theme);
  try { localStorage.setItem('dream-site-theme', theme); } catch { /* Storage is optional. */ }
});

function closeMenu() {
  byId('docs-menu').hidden = true;
  byId('menu-toggle').setAttribute('aria-expanded', 'false');
}
byId('menu-toggle').addEventListener('click', () => {
  const expanded = byId('menu-toggle').getAttribute('aria-expanded') !== 'true';
  byId('menu-toggle').setAttribute('aria-expanded', String(expanded));
  byId('docs-menu').hidden = !expanded;
});
byId('topic-search').addEventListener('input', event => {
  const query = event.target.value.trim().toLocaleLowerCase();
  let count = 0;
  document.querySelectorAll('.topic-group').forEach(group => {
    let visible = 0;
    group.querySelectorAll('.topic-link').forEach(link => {
      link.hidden = !`${group.querySelector('h2').textContent} ${link.textContent}`.toLocaleLowerCase().includes(query);
      if (!link.hidden) visible++;
    });
    group.hidden = visible === 0;
    count += visible;
  });
  byId('search-status').textContent = query ? `${count} matching topic${count === 1 ? '' : 's'}` : '';
});

const exampleTabs = ['hello', 'functions'];
function selectExample(name) {
  exampleTabs.forEach(id => {
    const selected = name === id;
    byId(`${id}-tab`).setAttribute('aria-selected', String(selected));
    byId(`${id}-tab`).tabIndex = selected ? 0 : -1;
    byId(`${id}-panel`).hidden = !selected;
  });
  byId('example-filename').textContent = name === 'hello' ? 'hello.dr' : 'functions.dr';
}
exampleTabs.forEach((name, index) => {
  const tab = byId(`${name}-tab`);
  tab.addEventListener('click', () => selectExample(name));
  tab.addEventListener('keydown', event => {
    if (!['ArrowLeft', 'ArrowRight', 'Home', 'End'].includes(event.key)) return;
    event.preventDefault();
    const next = event.key === 'Home' ? 'hello' : event.key === 'End' ? 'functions' : exampleTabs[1 - index];
    selectExample(next);
    byId(`${next}-tab`).focus();
  });
});

function highlightDream(code) {
  const source = code.textContent;
  const tokens = /\/\/[^\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|\b(?:func|return|int|float|bool|char|string|if|else|while|for|class|new|const|true|false|null)\b/g;
  let offset = 0;
  const fragment = document.createDocumentFragment();
  for (const match of source.matchAll(tokens)) {
    fragment.append(document.createTextNode(source.slice(offset, match.index)));
    const kind = match[0].startsWith('//') ? 'comment' : /^["']/.test(match[0]) ? 'string' : 'keyword';
    fragment.append(element('span', match[0], `syntax-${kind}`));
    offset = match.index + match[0].length;
  }
  fragment.append(document.createTextNode(source.slice(offset)));
  code.replaceChildren(fragment);
}

function decorateCode() {
  article.querySelectorAll('pre > code').forEach(code => {
    if (code.matches('.language-dream, .language-dr')) highlightDream(code);
    const button = element('button', 'Copy code', 'copy-button');
    button.type = 'button';
    button.addEventListener('click', async () => {
      try {
        await navigator.clipboard.writeText(code.textContent);
        button.textContent = 'Copied';
        status.textContent = 'Code copied to clipboard.';
      } catch {
        const range = document.createRange();
        range.selectNodeContents(code);
        const selection = window.getSelection();
        selection.removeAllRanges();
        selection.addRange(range);
        button.textContent = 'Code selected';
        status.textContent = 'Clipboard access is unavailable. The code is selected for manual copying.';
      }
      setTimeout(() => { if (button.isConnected) button.textContent = 'Copy code'; }, 2000);
    });
    code.parentElement.append(button);
  });
}

function buildOutline() {
  const toc = byId('toc');
  toc.replaceChildren();
  const used = new Set();
  article.querySelectorAll('h1,h2,h3,h4,h5,h6').forEach(heading => {
    const slug = headingSlug(heading.textContent);
    let id = slug;
    let suffix = 1;
    while (used.has(id)) id = `${slug}-${suffix++}`;
    used.add(id);
    heading.id = id;
    if (heading.tagName !== 'H2') return;
    const link = element('a', heading.textContent);
    link.href = docURL(activePath, id, base);
    link.dataset.heading = id;
    toc.append(link);
  });
  if (!toc.childElementCount) toc.append(element('span', 'No sections on this page.'));
  const links = Array.from(toc.querySelectorAll('a'));
  if ('IntersectionObserver' in window) {
    outlineObserver = new IntersectionObserver(entries => {
      const visible = entries.find(entry => entry.isIntersecting);
      if (!visible) return;
      links.forEach(link => {
        if (link.dataset.heading === visible.target.id) link.setAttribute('aria-current', 'location');
        else link.removeAttribute('aria-current');
      });
    }, { rootMargin: '0px 0px -65% 0px' });
    article.querySelectorAll('h2').forEach(heading => outlineObserver.observe(heading));
  }
}

function decorateLinks() {
  article.querySelectorAll('a[href]').forEach(link => {
    link.href = resolveContentLink(link.getAttribute('href'), activePath, base);
    if (new URL(link.href).origin !== location.origin) link.rel = 'noreferrer';
  });
  article.querySelectorAll('img[src]').forEach(img => {
    const src = img.getAttribute('src');
    if (/^[a-z][a-z\d+.-]*:|^\/\//i.test(src)) return;
    const path = new URL(src, `https://repository.invalid/docs/${activePath}`).pathname.slice(1);
    img.src = path.startsWith('docs/') ? new URL(path.slice(5), base) : `https://raw.githubusercontent.com/Ddemon26/DreamCompiler/${sourceBranch}/${path}`;
  });
  article.querySelectorAll('table').forEach(table => {
    const wrapper = element('div', undefined, 'table-wrap');
    table.before(wrapper);
    wrapper.append(table);
  });
}

function buildPagination() {
  const pagination = byId('page-pagination');
  pagination.replaceChildren();
  const index = pages.findIndex(page => page.path === activePath);
  if (index < 0) return;
  [[index - 1, 'Previous'], [index + 1, 'Next']].forEach(([position, label]) => {
    const page = pages[position];
    if (!page) return;
    const link = element('a');
    link.href = docURL(page.path, '', base);
    link.dataset.direction = label.toLowerCase();
    link.append(element('small', label), document.createTextNode(`${label === 'Previous' ? '← ' : ''}${page.title}${label === 'Next' ? ' →' : ''}`));
    pagination.append(link);
  });
}

function revealAnchor() {
  let id;
  try { id = decodeURIComponent(location.hash.slice(1)); } catch { return; }
  if (id) document.getElementById(id)?.scrollIntoView({ block: 'start' });
}

function loadRenderer() {
  if (!renderer) renderer = Promise.all([import('./vendor/marked.esm.js'), import('./vendor/purify.es.js')]).catch(error => { renderer = undefined; throw error; });
  return renderer;
}

async function renderRoute(focus = false) {
  const path = new URL(location.href).searchParams.get('doc') || '';
  if (path && path === activePath && article.childElementCount && !pendingRequest) { revealAnchor(); return; }
  pendingRequest?.abort();
  pendingRequest = undefined;
  outlineObserver?.disconnect();
  activePath = path;
  byId('home-view').hidden = Boolean(path);
  byId('docs-view').hidden = !path;
  byId('brand-context').hidden = !path;
  if (path) byId('docs-link').setAttribute('aria-current', 'page');
  else byId('docs-link').removeAttribute('aria-current');
  if (!path) {
    document.title = 'Dream — a language taking shape';
    if (location.hash) requestAnimationFrame(revealAnchor);
    else if (focus) window.scrollTo(0, 0);
    return;
  }
  closeMenu();
  // Restore all topics when a document is selected after filtering.
  byId('topic-search').value = '';
  byId('topic-search').dispatchEvent(new Event('input'));
  article.replaceChildren();
  byId('toc').replaceChildren();
  byId('page-pagination').replaceChildren();
  article.setAttribute('aria-busy', 'true');
  document.querySelectorAll('.topic-link').forEach(link => {
    if (link.dataset.path === path) link.setAttribute('aria-current', 'page');
    else link.removeAttribute('aria-current');
  });
  const topic = pages.find(page => page.path === path);
  const section = path.startsWith('reference/v1.') ? 'Historical archive' : topic?.group || 'Documentation';
  byId('breadcrumb-section').textContent = section;
  byId('article-section').textContent = section;
  byId('source-link').hidden = !validDocPath(path);
  byId('github-source-link').hidden = !validDocPath(path);
  if (validDocPath(path)) {
    byId('source-link').href = new URL(path, base);
    byId('github-source-link').href = githubURL(`docs/${path}`);
  }
  status.textContent = 'Loading documentation…';
  const request = new AbortController();
  pendingRequest = request;
  try {
    if (!validDocPath(path)) throw new Error('Invalid documentation path.');
    const [response, [{ marked }, { default: DOMPurify }]] = await Promise.all([fetch(new URL(path, base), { signal: request.signal }), loadRenderer()]);
    if (!response.ok) throw new Error(`The page returned HTTP ${response.status}.`);
    const markdown = await response.text();
    if (request.signal.aborted) return;
    article.innerHTML = DOMPurify.sanitize(marked.parse(markdown), {
      USE_PROFILES: { html: true }, FORBID_TAGS: ['style', 'form', 'input', 'iframe'], FORBID_ATTR: ['style'], SANITIZE_NAMED_PROPS: true,
    });
    decorateCode();
    decorateLinks();
    buildOutline();
    buildPagination();
    document.title = `${article.querySelector('h1')?.textContent || topic?.title || 'Documentation'} — Dream`;
    status.textContent = /^reference\/v1\.[01]\//.test(path)
      ? 'Historical documentation. Examples may describe planned or unqualified behavior; check implementation status.' : '';
    if (location.hash) requestAnimationFrame(revealAnchor);
    else if (focus) window.scrollTo(0, 0);
    if (focus) byId('main').focus({ preventScroll: true });
  } catch (error) {
    if (request.signal.aborted) return;
    document.title = 'Documentation unavailable — Dream';
    article.append(element('h1', 'This page is unavailable'), element('p', 'Check the address, choose another topic, or try loading the page again.'));
    const retry = element('button', 'Try again', 'button');
    retry.type = 'button';
    retry.addEventListener('click', () => { activePath = ''; renderRoute(true); });
    article.append(retry);
    status.textContent = error.message;
  } finally {
    if (pendingRequest === request) {
      pendingRequest = undefined;
      article.setAttribute('aria-busy', 'false');
    }
  }
}

document.addEventListener('click', event => {
  if (event.defaultPrevented || event.button !== 0 || event.metaKey || event.ctrlKey || event.shiftKey || event.altKey) return;
  const link = event.target.closest('a[href]');
  if (!link || link.target || link.hasAttribute('download') || link.classList.contains('skip-link')) return;
  const url = new URL(link.href);
  if (url.origin !== base.origin || ![base.pathname, base.pathname + 'index.html'].includes(url.pathname)) return;
  event.preventDefault();
  history.pushState(null, '', url);
  renderRoute(true);
});
window.addEventListener('popstate', () => renderRoute(false));
window.addEventListener('hashchange', revealAnchor);
buildNavigation();
closeMenu();
renderRoute();
