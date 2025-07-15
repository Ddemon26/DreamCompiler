const fs = require('fs');
// Canonical token definitions live at the repository root
const TOKEN_FILE = 'tokens.json';
const tokens = JSON.parse(fs.readFileSync(TOKEN_FILE, 'utf8'));

// TextMate grammar
const tmGrammar = {
  '$schema': 'https://raw.githubusercontent.com/microsoft/vscode/master/extensions/theme-defaults/test/colorize-fixtures/TMGrammarSchema.json',
  scopeName: 'source.dream',
  patterns: [{include:'#tokens'}],
  repository: { tokens: { patterns: tokens.map(t=>({name:t.scope, match:t.regex})) } }
};
fs.mkdirSync('vscode/syntaxes', { recursive: true });
fs.writeFileSync('vscode/syntaxes/dream.tmLanguage.json', JSON.stringify(tmGrammar, null, 2));

// JFlex lexer
let flex = `package com.dream;\n\n%%\n%public\n%class DreamLexer\n%implements com.intellij.lexer.FlexLexer\n%unicode\n%function advance\n%type com.intellij.psi.tree.IElementType\n\n%%\n<YYINITIAL> {\n`;
function escapeFlex(re){
  return re.replace(/\\/g, "\\\\").replace(/"/g, '\\"');
}
for(const t of tokens){
  const escaped = escapeFlex(t.regex);
  const pattern = t.regex.startsWith('\/\\*') ? `"${escaped}"` : escaped;
  flex += `  ${pattern} { return DreamTokenTypes.${t.name.toUpperCase()}; }\n`;
}
flex += '  [\\t\\r\\n ]+ { return com.intellij.psi.TokenType.WHITE_SPACE; }\n  . { return com.intellij.psi.TokenType.BAD_CHARACTER; }\n}\n';
fs.mkdirSync('idea/src/main/java/com/dream', { recursive: true });
fs.writeFileSync('idea/src/main/java/com/dream/DreamLexer.flex', flex);

// Keep JetBrains tokens.json in sync
fs.mkdirSync('idea/src/main/resources', { recursive: true });
fs.writeFileSync('idea/src/main/resources/tokens.json', JSON.stringify(tokens, null, 2));
// Mirror token file into the plugin root if not a symlink
if (!fs.existsSync('idea/tokens.json') ||
    !fs.lstatSync('idea/tokens.json').isSymbolicLink()) {
  fs.writeFileSync('idea/tokens.json', JSON.stringify(tokens, null, 2));
}
