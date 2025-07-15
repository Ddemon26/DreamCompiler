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
  \\b(if|else|while|for|do|break|continue|return|int|string|bool|true|false|func|Console|WriteLine)\\b { return DreamTokenTypes.KEYWORD; }
  \\b\\d+\\b { return DreamTokenTypes.NUMBER; }
  \"([^\\\"\\n]|\\\\.)*\" { return DreamTokenTypes.STRING; }
  \/\/.* { return DreamTokenTypes.LINE_COMMENT; }
  "/\\*[^*]*\\*+([^/*][^*]*\\*+)*/" { return DreamTokenTypes.BLOCK_COMMENT; }
  , { return DreamTokenTypes.COMMA; }
  [\t\r\n ]+ { return com.intellij.psi.TokenType.WHITE_SPACE; }
  . { return com.intellij.psi.TokenType.BAD_CHARACTER; }
}
