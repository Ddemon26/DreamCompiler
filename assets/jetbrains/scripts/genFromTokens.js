const fs = require('fs');
const path = require('path');

// Plugin root lives one directory up from this script
const pluginDir = path.resolve(__dirname, '..');
// Repository root is two directories up from this script
const repoRoot = path.resolve(pluginDir, '..', '..');

// Source of truth for tokens used by the compiler
const TOKENS_DEF = path.join(repoRoot, 'src', 'lexer', 'tokens.def');

function parseTokensDef(content) {
  const map = {};
  // Match lines of the form: TOKEN(NAME, "value")
  //   - ([^,]+) captures the token name
  //   - \s* consumes optional whitespace after the comma
  //   - "((?:\\.|[^"])*)" captures the value string allowing escapes
  const re = /TOKEN\(([^,]+),\s*"((?:\\.|[^"])*)"\)/g;
  for (const m of content.matchAll(re)) {
    const raw = m[2];
    const js = '"' + raw.replace(/\\/g, '\\\\').replace(/"/g, '\\"') + '"';
    map[m[1]] = JSON.parse(js);
  }
  return map;
}

const defs = parseTokensDef(fs.readFileSync(TOKENS_DEF, 'utf8'));

const kwRegex = Object.entries(defs)
  .filter(([name]) => name.startsWith('KW_'))
  .map(([, re]) => re)
  .join('|');

const operatorNames = Object.keys(defs)
  .filter(name =>
    !name.startsWith('KW_') &&
    !name.endsWith('_LITERAL') &&
    ![
      'IDENT','SEMICOLON','COMMA','DOT','LPAREN','RPAREN',
      'LBRACE','RBRACE','LBRACKET','RBRACKET','COLON','DCOLON',
      'ARROW','FATARROW','EOF','ERROR'
    ].includes(name)
  );
const opRegex = operatorNames
  .map(n => {
    if (!defs[n]) throw new Error(`Missing operator definition for: ${n}`);
    return defs[n];
  })
  .join('|');

const tokens = [
  {
    name: 'keyword',
    regex: `\\b(${kwRegex})\\b`,
    // JFlex uses Java-style regexes. To get a word boundary we need to double
    // escape the backslash so the generated .flex file contains `\b`.
    flex: `\\b(${kwRegex})\\b`,
    scope: 'keyword.control',
  },
  // JFlex does not support non-capturing groups, so use a normal group instead
  {
    name: 'number',
    regex: `\\b(${defs.FLOAT_LITERAL}|${defs.INT_LITERAL})\\b`,
    flex: `\\b(${defs.FLOAT_LITERAL}|${defs.INT_LITERAL})\\b`,
    scope: 'constant.numeric',
  },
  { name: 'string', regex: defs.STRING_LITERAL, scope: 'string.quoted.double' },
  { name: 'doc_comment', regex: '/\\*\\*[^]*?\\*/|///.*', scope: 'comment.block.documentation' },
  { name: 'line_comment', regex: '//.*', scope: 'comment.line.double-slash' },
  { name: 'block_comment', regex: '/\\*[\\s\\S]*?\\*/', scope: 'comment.block' },
  {
    name: 'identifier',
    regex: `\\b${defs.IDENT}\\b`,
    flex: `\\b${defs.IDENT}\\b`,
    scope: 'variable.other',
  },
  { name: 'operator', regex: opRegex, scope: 'keyword.operator' },
  { name: 'semicolon', regex: defs.SEMICOLON, scope: 'punctuation.terminator.statement' },
  { name: 'comma', regex: defs.COMMA, scope: 'punctuation.separator.comma' },
  { name: 'dot', regex: defs.DOT, scope: 'punctuation.accessor' },
  { name: 'paren', regex: '[()]', scope: 'punctuation.section.parens' },
  { name: 'brace', regex: '[{}]', scope: 'punctuation.section.braces' },
  { name: 'bracket', regex: '[\\\\[\\\\]]', scope: 'punctuation.section.brackets' },
];

// TextMate grammar
const tmGrammar = {
  '$schema': 'https://raw.githubusercontent.com/microsoft/vscode/master/extensions/theme-defaults/test/colorize-fixtures/TMGrammarSchema.json',
  scopeName: 'source.dream',
  patterns: [{include:'#tokens'}],
  repository: { tokens: { patterns: tokens.map(t=>({name:t.scope, match:t.regex})) } }
};
const vscodeDir = path.join(repoRoot, 'assets', 'vscode', 'syntaxes');
fs.mkdirSync(vscodeDir, { recursive: true });
fs.writeFileSync(path.join(vscodeDir, 'dream.tmLanguage.json'), JSON.stringify(tmGrammar, null, 2));

// JFlex lexer
let flex = `package com.dream;\n\n%%\n%public\n%class DreamLexer\n%implements com.intellij.lexer.FlexLexer\n%unicode\n%function advance\n%type com.intellij.psi.tree.IElementType\n\n%%\n<YYINITIAL> {\n`;
function escapeFlex(re){
  return re
    .replace(/\/\//g, '\\/\\/')
    .replace(/\/\*/g, '\\/\\*')
    .replace(/\*\//g, '\\*\\/');
}
for(const t of tokens){
  const raw = t.flex || t.regex;
  let pattern = escapeFlex(raw);
  if (t.name === 'operator') {
    const ops = operatorNames.map(n => escapeFlex(defs[n]));
    pattern = ops.map(op => `"${op}"`).join('|');
  } else if (raw.startsWith('/\\*')) {
    pattern = `"${pattern}"`;
  }
  flex += `  ${pattern} { return DreamTokenTypes.${t.name.toUpperCase()}; }\n`;
}
flex += '  [\\t\\r\\n ]+ { return com.intellij.psi.TokenType.WHITE_SPACE; }\n  . { return com.intellij.psi.TokenType.BAD_CHARACTER; }\n}\n';
fs.mkdirSync(path.join(pluginDir, 'src', 'main', 'java', 'com', 'dream'), { recursive: true });
fs.writeFileSync(path.join(pluginDir, 'src', 'main', 'java', 'com', 'dream', 'DreamLexer.flex'), flex);

// Keep JetBrains tokens.json in sync
fs.mkdirSync(path.join(pluginDir, 'src', 'main', 'resources'), { recursive: true });
fs.writeFileSync(path.join(pluginDir, 'src', 'main', 'resources', 'tokens.json'), JSON.stringify(tokens, null, 2));
// Mirror token file into the plugin root if not a symlink
const mirror = path.join(pluginDir, 'tokens.json');
if (!fs.existsSync(mirror) || !fs.lstatSync(mirror).isSymbolicLink()) {
  fs.writeFileSync(mirror, JSON.stringify(tokens, null, 2));
}
