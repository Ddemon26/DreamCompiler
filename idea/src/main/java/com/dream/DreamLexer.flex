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
  \\b(if|else|while|for|do|break|continue|return|int|string|bool|true|false|func|Console|WriteLine|switch|case|default)\\b { return DreamTokenTypes.KEYWORD; }
  \\b\\d+\\b { return DreamTokenTypes.NUMBER; }
  \"([^\\\"\\n]|\\\\.)*\" { return DreamTokenTypes.STRING; }
  "/*""**"([^*]|"*"+[^/*])*"*"+"/" | "///".* { return DreamTokenTypes.COMMENTDOC; }
  "//".* { return DreamTokenTypes.COMMENT; }
  "/*"[^*]*"*"+([^/*][^*]*"*"+)*"/" { return DreamTokenTypes.COMMENTBLOCK; }
  [a-zA-Z_][a-zA-Z0-9_]* { return DreamTokenTypes.IDENTIFIER; }
  "++"|"--"|"+="|"-="|"*="|"/="|"%="|"+"|"-"|"*"|"/"|"%"|"<="|">="|"=="|"!="|"<"|">"|"&&"|"||"|"!"|"="|"?" { return DreamTokenTypes.OPERATOR; }
  ; { return DreamTokenTypes.SEMICOLON; }
  , { return DreamTokenTypes.COMMA; }
  \\. { return DreamTokenTypes.DOT; }
  [()] { return DreamTokenTypes.PAREN; }
  [{}] { return DreamTokenTypes.BRACE; }
  [\\\\[\\\\]] { return DreamTokenTypes.BRACKET; }
  [\t\r\n ]+ { return com.intellij.psi.TokenType.WHITE_SPACE; }
  . { return com.intellij.psi.TokenType.BAD_CHARACTER; }
}
