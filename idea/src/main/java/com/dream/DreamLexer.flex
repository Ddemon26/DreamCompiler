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
  \\b(if|else|while|for|do|break|continue|return|int|string|func|Console|WriteLine)\\b { return DreamTokenTypes.KEYWORD; }
  \\b\\d+\\b { return DreamTokenTypes.NUMBER; }
  \"([^\\\"\\n]|\\\\.)*\" { return DreamTokenTypes.STRING; }
  //.* { return DreamTokenTypes.COMMENT; }
  "/\\*[^*]*\\*+([^/*][^*]*\\*+)*/" { return DreamTokenTypes.COMMENTBLOCK; }
  , { return DreamTokenTypes.COMMA; }
  [\t\r\n ]+ { return com.intellij.psi.TokenType.WHITE_SPACE; }
  . { return com.intellij.psi.TokenType.BAD_CHARACTER; }
}
