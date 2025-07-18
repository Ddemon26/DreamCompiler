package com.dream;

import com.intellij.psi.tree.IElementType;

public final class DreamTokenTypes {
    private DreamTokenTypes() {}

    public static final IElementType KEYWORD = new DreamElementType("KEYWORD");
    public static final IElementType NUMBER = new DreamElementType("NUMBER");
    public static final IElementType STRING = new DreamElementType("STRING");
    public static final IElementType LINE_COMMENT = new DreamElementType("LINE_COMMENT");
    public static final IElementType BLOCK_COMMENT = new DreamElementType("BLOCK_COMMENT");
    public static final IElementType DOC_COMMENT = new DreamElementType("DOC_COMMENT");
    public static final IElementType OPERATOR = new DreamElementType("OPERATOR");
    public static final IElementType SEMICOLON = new DreamElementType("SEMICOLON");
    public static final IElementType COMMA = new DreamElementType("COMMA");
    public static final IElementType DOT = new DreamElementType("DOT");
    public static final IElementType PAREN = new DreamElementType("PAREN");
    public static final IElementType BRACE = new DreamElementType("BRACE");
    public static final IElementType BRACKET = new DreamElementType("BRACKET");
    public static final IElementType IDENTIFIER = new DreamElementType("IDENTIFIER");
}
