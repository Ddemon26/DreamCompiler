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
}
