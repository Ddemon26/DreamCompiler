package com.dream;

import com.intellij.psi.tree.IElementType;

public final class DreamTokenTypes {
    private DreamTokenTypes() {}

    public static final IElementType KEYWORD = new DreamElementType("KEYWORD");
    public static final IElementType KEYWORDTYPE = new DreamElementType("KEYWORDTYPE");
    public static final IElementType KEYWORDOTHER = new DreamElementType("KEYWORDOTHER");
    public static final IElementType KEYWORDLITERAL = new DreamElementType("KEYWORDLITERAL");
    public static final IElementType NUMBER = new DreamElementType("NUMBER");
    public static final IElementType STRING = new DreamElementType("STRING");
    public static final IElementType CHAR = new DreamElementType("CHAR");
    public static final IElementType COMMENT = new DreamElementType("COMMENT");
    public static final IElementType COMMENTBLOCK = new DreamElementType("COMMENTBLOCK");
    public static final IElementType CONSOLEFUNCTION = new DreamElementType("CONSOLEFUNCTION");
    public static final IElementType OPERATORARITHMETIC = new DreamElementType("OPERATORARITHMETIC");
    public static final IElementType OPERATORBITWISE = new DreamElementType("OPERATORBITWISE");
    public static final IElementType OPERATORCOMPARISON = new DreamElementType("OPERATORCOMPARISON");
    public static final IElementType OPERATORLOGICAL = new DreamElementType("OPERATORLOGICAL");
    public static final IElementType OPERATORASSIGNMENT = new DreamElementType("OPERATORASSIGNMENT");
    public static final IElementType OPERATORCONDITIONAL = new DreamElementType("OPERATORCONDITIONAL");
    public static final IElementType OPERATORARROW = new DreamElementType("OPERATORARROW");
    public static final IElementType SEMICOLON = new DreamElementType("SEMICOLON");
    public static final IElementType COMMA = new DreamElementType("COMMA");
    public static final IElementType DOT = new DreamElementType("DOT");
    public static final IElementType PAREN = new DreamElementType("PAREN");
    public static final IElementType BRACE = new DreamElementType("BRACE");
    public static final IElementType IDENTIFIER = new DreamElementType("IDENTIFIER");
    public static final IElementType BRACKET = new DreamElementType("BRACKET");
    public static final IElementType COMMENTDOC = new DreamElementType("COMMENTDOC");
}
