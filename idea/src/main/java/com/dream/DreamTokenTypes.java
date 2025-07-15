package com.dream;

import com.intellij.psi.tree.IElementType;
import com.intellij.psi.TokenType;
import com.intellij.psi.tree.TokenSet;
import com.intellij.lang.Language;

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
}
