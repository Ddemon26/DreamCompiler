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
  "if"|"else"|"while"|"for"|"do"|"break"|"continue"|"return"|"switch"|"case"|"default"|"try"|"catch"|"finally"|"throw" { return DreamTokenTypes.KEYWORD; }
  "int"|"string"|"bool"|"float"|"char"|"void" { return DreamTokenTypes.KEYWORDTYPE; }
  "class"|"struct"|"enum" { return DreamTokenTypes.KEYWORDCLASS; }
  "public"|"private"|"static"|"const"|"new"|"func"|"var"|"let"|"base"|"using"|"import"|"module"|"export"|"async"|"await"|"Task"|"TaskResult" { return DreamTokenTypes.KEYWORDOTHER; }
  "true"|"false"|"null" { return DreamTokenTypes.KEYWORDLITERAL; }
  [0-9]+\.[0-9]+|[0-9]+ { return DreamTokenTypes.NUMBER; }
  \"([^\\\"\n]|\\.)*\" { return DreamTokenTypes.STRING; }
  '([^'\n]|\\.)' { return DreamTokenTypes.CHAR; }
  "/**"[^]*?"*/"|"///"[^\r\n]* { return DreamTokenTypes.COMMENTDOC; }
  "//"[^\r\n]* { return DreamTokenTypes.COMMENT; }
  "/*"[^]*?"*/" { return DreamTokenTypes.COMMENTBLOCK; }
  Console.WriteLine|Console.Write|Console.ReadLine { return DreamTokenTypes.CONSOLEFUNCTION; }
  (class|struct)[ \t]+[a-zA-Z_][a-zA-Z0-9_]* { return DreamTokenTypes.CLASSDECLARATION; }
  "func"[ \t]+((int|string|bool|float|char|void|const)[ \t]+)?[a-zA-Z_][a-zA-Z0-9_]*[ \t]*/"(" { return DreamTokenTypes.FUNCTIONDECLARATION; }
  [a-zA-Z_][a-zA-Z0-9_]*[ \t]*/"(" { return DreamTokenTypes.FUNCTIONCALL; }
  (const[ \t]+)?(int|string|bool|float|char|void)[ \t]+[a-zA-Z_][a-zA-Z0-9_]* { return DreamTokenTypes.PARAMETERNAME; }
  [a-zA-Z_][a-zA-Z0-9_]* { return DreamTokenTypes.IDENTIFIER; }
  "\+\+"|"--"|"\+="|"-="|"\*="|"/="|"%="|"\+"|"-"|"\*"|"/"|"%" { return DreamTokenTypes.OPERATORARITHMETIC; }
  "&="|"\|="|"\^="|"<<="|">>="|"&"|"\|"|"\^"|"<<"|">>"|"~" { return DreamTokenTypes.OPERATORBITWISE; }
  "<="|">="|"=="|"!="|"<"|">" { return DreamTokenTypes.OPERATORCOMPARISON; }
  "&&"|"\|\|"|"!" { return DreamTokenTypes.OPERATORLOGICAL; }
  "=" { return DreamTokenTypes.OPERATORASSIGNMENT; }
  "\?"|"\?\?"|"\?\?=" { return DreamTokenTypes.OPERATORCONDITIONAL; }
  "->"|"=>" { return DreamTokenTypes.OPERATORARROW; }
  ";" { return DreamTokenTypes.SEMICOLON; }
  "," { return DreamTokenTypes.COMMA; }
  "." { return DreamTokenTypes.DOT; }
  "("|")" { return DreamTokenTypes.PAREN; }
  "{"|"}" { return DreamTokenTypes.BRACE; }
  "["|"]" { return DreamTokenTypes.BRACKET; }
  [\t\r\n ]+ { return com.intellij.psi.TokenType.WHITE_SPACE; }
  . { return com.intellij.psi.TokenType.BAD_CHARACTER; }
}
