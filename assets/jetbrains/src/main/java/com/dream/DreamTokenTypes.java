package com.dream;

import com.intellij.psi.tree.IElementType;

public final class DreamTokenTypes {
    private DreamTokenTypes() {}

    public static final IElementType KEYWORD = new DreamElementType("KEYWORD");
    public static final IElementType NUMBER = new DreamElementType("NUMBER");
    public static final IElementType STRING = new DreamElementType("STRING");
    public static final IElementType COMMENT = new DreamElementType("COMMENT");
    public static final IElementType COMMENTBLOCK = new DreamElementType("COMMENTBLOCK");
    public static final IElementType OPERATOR = new DreamElementType("OPERATOR");
    public static final IElementType SEMICOLON = new DreamElementType("SEMICOLON");
    public static final IElementType COMMA = new DreamElementType("COMMA");
    public static final IElementType DOT = new DreamElementType("DOT");
    public static final IElementType PAREN = new DreamElementType("PAREN");
    public static final IElementType BRACE = new DreamElementType("BRACE");
    public static final IElementType FUNCTION = new DreamElementType("FUNCTION");
    public static final IElementType IDENTIFIER = new DreamElementType("IDENTIFIER");
    public static final IElementType BRACKET = new DreamElementType("BRACKET");
    public static final IElementType COMMENTDOC = new DreamElementType("COMMENTDOC");
}
