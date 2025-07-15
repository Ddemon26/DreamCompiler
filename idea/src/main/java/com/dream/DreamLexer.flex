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
  "//".* { return DreamTokenTypes.COMMENT; }
  "/\\*[^*]*\\*+([^/*][^*]*\\*+)*/" { return DreamTokenTypes.COMMENTBLOCK; }

  "if" | "else" | "while" | "for" | "do" | "break" | "continue" | "return" | "int" | "string" | "bool" | "true" | "false" | "func" | "Console" | "WriteLine" { return DreamTokenTypes.KEYWORD; }
  [0-9]+ { return DreamTokenTypes.NUMBER; }
  \"([^\\\"\r\n]|\\[^\r\n])*\" { return DreamTokenTypes.STRING; }

  "++" | "--" | "+" | "-" | "*" | "/" | "%" | "<=" | ">=" | "==" | "!=" | "<" | ">" | "&&" | "||" | "!" | "=" | "|" { return DreamTokenTypes.OPERATOR; }

  ";" { return DreamTokenTypes.SEMICOLON; }
  "," { return DreamTokenTypes.COMMA; }
  "." { return DreamTokenTypes.DOT; }
  "(" | ")" { return DreamTokenTypes.PAREN; }
  "{" | "}" { return DreamTokenTypes.BRACE; }

  [\t\r\n ]+ { return com.intellij.psi.TokenType.WHITE_SPACE; }
  . { return com.intellij.psi.TokenType.BAD_CHARACTER; }
}