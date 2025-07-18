package com.dream;

%%
%public
%class DreamLexer
%implements com.intellij.lexer.FlexLexer
%unicode
%function advance
%type com.intellij.psi.tree.IElementType

%%
<YYINITIAL> {
  (if|else|while|for|do|break|continue|return|class|struct|int|string|bool|float|char|void|var|true|false|func|switch|case|default|Console|WriteLine|Write|ReadLine)(?![A-Za-z0-9_]) { return DreamTokenTypes.KEYWORD; }
  ([0-9]+\\.[0-9]+|[0-9]+)(?![A-Za-z0-9_]) { return DreamTokenTypes.NUMBER; }
  \"([^\\\"\\n]|\\\\.)*\" { return DreamTokenTypes.STRING; }
  "/\*\*[^]*?\\*\/|\/\//.*" { return DreamTokenTypes.COMMENTDOC; }
  \/\/.* { return DreamTokenTypes.COMMENT; }
  "/\*[\s\S]*?\\*\/" { return DreamTokenTypes.COMMENTBLOCK; }
  [a-zA-Z_][a-zA-Z0-9_]*(?![A-Za-z0-9_]) { return DreamTokenTypes.IDENTIFIER; }
  "\\+\\+"|"--"|"\\+="|"-="|"\\*="|"/="|"%="|"&="|"\\|="|"\\^="|"<<="|">>="|"\\+"|"-"|"\\*"|"/"|"%"|"\\^"|"<<"|">>"|"<="|">="|"=="|"!="|"<"|">"|"&&"|"\\|\\|"|"&"|"\\|"|"~"|"!"|"="|"\\?"|"\\?\\?"|"\\?\\?=" { return DreamTokenTypes.OPERATOR; }
  ; { return DreamTokenTypes.SEMICOLON; }
  , { return DreamTokenTypes.COMMA; }
  \\. { return DreamTokenTypes.DOT; }
  [()] { return DreamTokenTypes.PAREN; }
  [{}] { return DreamTokenTypes.BRACE; }
  [\\[\\]] { return DreamTokenTypes.BRACKET; }
  [\t\r\n ]+ { return com.intellij.psi.TokenType.WHITE_SPACE; }
  . { return com.intellij.psi.TokenType.BAD_CHARACTER; }
}
