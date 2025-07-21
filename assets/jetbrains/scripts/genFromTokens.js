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

// Separate different types of keywords for better highlighting
const controlKeywords = ['if', 'else', 'while', 'for', 'do', 'break', 'continue', 'return', 'switch', 'case', 'default', 'try', 'catch', 'finally', 'throw'];
const controlKwRegex = controlKeywords.join('|');

const typeKeywords = ['int', 'string', 'bool', 'float', 'char', 'void'];
const typeKwRegex = typeKeywords.join('|');

const modifierKeywords = ['class', 'struct', 'static', 'new', 'func', 'var', 'let', 'base', 'using', 'import', 'module', 'export', 'async', 'await', 'Task', 'TaskResult'];
const modifierKwRegex = modifierKeywords.join('|');

const literalKeywords = ['true', 'false'];
const literalKwRegex = literalKeywords.join('|');

const tokens = [
  {
    name: 'keyword',
    regex: `\\b(${controlKwRegex})\\b`,
    flex: `\b(${controlKwRegex})\b`,
    scope: 'keyword.control',
  },
  {
    name: 'keywordType',
    regex: `\\b(${typeKwRegex})\\b`,
    flex: `\b(${typeKwRegex})\b`,
    scope: 'storage.type',
  },
  {
    name: 'keywordOther',
    regex: `\\b(${modifierKwRegex})\\b`,
    flex: `\b(${modifierKwRegex})\b`,
    scope: 'keyword.other',
  },
  {
    name: 'keywordLiteral',
    regex: `\\b(${literalKwRegex})\\b`,
    flex: `\b(${literalKwRegex})\b`,
    scope: 'constant.language',
  },
  // JFlex does not support non-capturing groups, so use a normal group instead
  {
    name: 'number',
    regex: `\\b(${defs.FLOAT_LITERAL}|${defs.INT_LITERAL})\\b`,
    flex: `\b(${defs.FLOAT_LITERAL}|${defs.INT_LITERAL})\b`,
    scope: 'constant.numeric',
  },
  { name: 'string', regex: defs.STRING_LITERAL, scope: 'string.quoted.double' },
  { name: 'char', regex: defs.CHAR_LITERAL, scope: 'string.quoted.single' },
  { name: 'commentDoc', regex: '/\\*\\*[^]*?\\*/|///.*', scope: 'comment.block.documentation' },
  { name: 'comment', regex: '//.*', scope: 'comment.line.double-slash' },
  { name: 'commentBlock', regex: '/\\*[\\s\\S]*?\\*/', scope: 'comment.block' },
  // Special highlighting for Console methods
  { name: 'consoleFunction', regex: '\\b(Console)\\.(WriteLine|Write|ReadLine)\\b', scope: 'support.function' },
  {
    name: 'identifier',
    regex: `\\b${defs.IDENT}\\b`,
    flex: `\b${defs.IDENT}\b`,
    scope: 'variable.other',
  },
  // Split operators by type for better highlighting
  { name: 'operatorArithmetic', regex: '\\+\\+|--|\\+=|-=|\\*=|/=|%=|\\+|-|\\*|/|%', scope: 'keyword.operator.arithmetic' },
  { name: 'operatorBitwise', regex: '&=|\\|=|\\^=|<<=|>>=|&|\\||\\^|<<|>>|~', scope: 'keyword.operator.bitwise' },
  { name: 'operatorComparison', regex: '<=|>=|==|!=|<|>', scope: 'keyword.operator.comparison' },
  { name: 'operatorLogical', regex: '&&|\\|\\||!', scope: 'keyword.operator.logical' },
  { name: 'operatorAssignment', regex: '=', scope: 'keyword.operator.assignment' },
  { name: 'operatorConditional', regex: '\\?|\\?\\?|\\?\\?=', scope: 'keyword.operator.conditional' },
  { name: 'operatorArrow', regex: '->|=>', scope: 'keyword.operator.arrow' },
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
let flex = `package com.dream;\n\n%%\n%public\n%class DreamLexer\n%implements com.intellij.lexer.FlexLexer\n%unicode\n%function advance\n%type com.intellij.psi.tree.IElementType\n\n%%\n\n<YYINITIAL> {\n`;

// Generate proper JFlex patterns
for(const t of tokens){
  let pattern = '';
  
  if (t.name === 'keyword') {
    pattern = `"${controlKeywords.join('"|"')}"`;
  } else if (t.name === 'keywordType') {
    pattern = `"${typeKeywords.join('"|"')}"`;
  } else if (t.name === 'keywordOther') {
    pattern = `"${modifierKeywords.join('"|"')}"`;
  } else if (t.name === 'keywordLiteral') {
    pattern = `"${literalKeywords.join('"|"')}"`;
  } else if (t.name === 'number') {
    pattern = '[0-9]+\\.[0-9]+|[0-9]+';
  } else if (t.name === 'string') {
    pattern = '\\"([^\\\\\\"\\n]|\\\\.)*\\"';
  } else if (t.name === 'char') {
    pattern = "'([^'\\n]|\\\\.)'"
  } else if (t.name === 'commentDoc') {
    pattern = '"/**"[^]*?"*/"\\n"///"[^\\r\\n]*';
  } else if (t.name === 'comment') {
    pattern = '"//"[^\\r\\n]*';
  } else if (t.name === 'commentBlock') {
    pattern = '"/*"[^]*?"*/"';
  } else if (t.name === 'consoleFunction') {
    pattern = '"Console"."WriteLine"|"Console"."Write"|"Console"."ReadLine"';
  } else if (t.name === 'identifier') {
    pattern = '[a-zA-Z_][a-zA-Z0-9_]*';
  } else if (t.name.startsWith('operator')) {
    // Handle operators specially - quote each one
    const opRegex = t.regex;
    pattern = opRegex.split('|').map(op => `"${op}"`).join('|');
  } else if (t.name === 'semicolon') {
    pattern = '";"';
  } else if (t.name === 'comma') {
    pattern = '","';
  } else if (t.name === 'dot') {
    pattern = '"."';
  } else if (t.name === 'paren') {
    pattern = '"|"("')';
  } else if (t.name === 'brace') {
    pattern = '"{"|"}"';
  } else if (t.name === 'bracket') {
    pattern = '"["|"]"';
  } else {
    // Fallback - quote the regex
    pattern = `"${t.regex.replace(/\\/g, '\\\\').replace(/"/g, '\\"')}"`;
  }
  
  flex += `  ${pattern} { return DreamTokenTypes.${t.name.toUpperCase()}; }\\n`;
}
flex += '  [\\t\\r\\n ]+ { return com.intellij.psi.TokenType.WHITE_SPACE; }\\n  . { return com.intellij.psi.TokenType.BAD_CHARACTER; }\\n}\\n';
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
