package com.dream;

import com.intellij.psi.tree.IElementType;

public interface DreamTokenTypes {
    IElementType KEYWORD = new DreamElementType("KEYWORD");
    IElementType NUMBER = new DreamElementType("NUMBER");
    IElementType STRING = new DreamElementType("STRING");
    IElementType COMMENT = new DreamElementType("COMMENT");
    IElementType COMMENTBLOCK = new DreamElementType("COMMENTBLOCK");
    IElementType OPERATOR = new DreamElementType("OPERATOR");
    IElementType SEMICOLON = new DreamElementType("SEMICOLON");
    IElementType COMMA = new DreamElementType("COMMA");
    IElementType DOT = new DreamElementType("DOT");
    IElementType PAREN = new DreamElementType("PAREN");
    IElementType BRACE = new DreamElementType("BRACE");
    IElementType IDENTIFIER = new DreamElementType("IDENTIFIER");
    IElementType BRACKET = new DreamElementType("BRACKET");
    IElementType COMMENTDOC = new DreamElementType("COMMENTDOC");
}
