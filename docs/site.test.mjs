import test from 'node:test';
import assert from 'node:assert/strict';
import { readFile, access } from 'node:fs/promises';
import { groups, pages, docURL, validDocPath, resolveContentLink, headingSlug } from './site-data.js';

const base = new URL('https://ddemon26.github.io/DreamCompiler/');

test('every sidebar entry names a real, unique Markdown document', async () => {
  assert.equal(new Set(pages.map(page => page.path)).size, pages.length);
  assert.ok(groups.length > 1);
  for (const page of pages) {
    assert.ok(validDocPath(page.path), page.path);
    await access(new URL(page.path, import.meta.url));
  }
});

test('document and heading links survive a repository subpath and query roundtrip', () => {
  const path = 'reference/grammar/Lexical grammar.md';
  const url = docURL(path, 'string-literals', base);
  assert.equal(url.pathname, '/DreamCompiler/');
  assert.equal(url.searchParams.get('doc'), path);
  assert.equal(url.hash, '#string-literals');
  assert.ok(validDocPath(path));
});

test('relative Markdown links use the source directory and preserve heading links', () => {
  const result = resolveContentLink('../language-features/functions.md#basic-functions', 'getting-started/quick-start.md', base);
  assert.equal(result.searchParams.get('doc'), 'language-features/functions.md');
  assert.equal(result.hash, '#basic-functions');
  const samePage = resolveContentLink('#compile-and-run', 'getting-started/quick-start.md', base);
  assert.equal(samePage.searchParams.get('doc'), 'getting-started/quick-start.md');
  assert.equal(samePage.hash, '#compile-and-run');
});

test('repository evidence leaves Pages for the approved GitHub source branch', () => {
  const result = resolveContentLink('../../codex/FEATURES.md', 'development/implementation-status.md', base);
  assert.equal(result.href, 'https://github.com/Ddemon26/DreamCompiler/blob/codex/compiler-stabilization/codex/FEATURES.md');
  const build = resolveContentLink('../../../build.zig', 'development/compiler/build.md', base);
  assert.equal(build.pathname, '/Ddemon26/DreamCompiler/blob/codex/compiler-stabilization/build.zig');
});

test('directory references and raw files keep their intended destinations', () => {
  assert.equal(resolveContentLink('grammar/', 'reference/README.md', base).searchParams.get('doc'), 'reference/grammar/Grammar.md');
  assert.equal(resolveContentLink('v1.1/', 'reference/README.md', base).searchParams.get('doc'), 'reference/README.md');
  assert.equal(resolveContentLink('https://ziglang.org/download/', 'index.md', base).href, 'https://ziglang.org/download/');
  assert.equal(resolveContentLink('image.png', 'getting-started/intro.md', base).pathname, '/DreamCompiler/getting-started/image.png');
});

test('invalid fetch paths fail closed without throwing', () => {
  for (const path of ['', '../README.md', '/index.md', 'a/../index.md', 'https://other.invalid/test.md', 'https://[bad', '//other.invalid/test.md', 'x\\index.md', 'index.md?x=1', 'index.md#anchor', '%2e%2e/README.md', 'a\n.md', 'file.html']) {
    assert.equal(validDocPath(path), false, path);
  }
});

test('section slugs support punctuation, Unicode and empty headings', () => {
  assert.equal(headingSlug('Compile and run'), 'compile-and-run');
  assert.equal(headingSlug('Console.WriteLine()'), 'consolewriteline');
  assert.equal(headingSlug('Déclarations & types'), 'déclarations-types');
  assert.equal(headingSlug('!!!'), 'section');
});

test('the approved square-edge rule is preserved by the stylesheet', async () => {
  const css = await readFile(new URL('index.css', import.meta.url), 'utf8');
  const radii = [...css.matchAll(/border-radius\s*:\s*([^;}]+)/g)].map(match => match[1].trim());
  assert.ok(radii.length);
  assert.ok(radii.every(value => value === '0'));
  assert.ok(!css.includes('box-shadow:'));
});
