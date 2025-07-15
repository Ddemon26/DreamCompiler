package com.dream;

import com.intellij.psi.tree.IElementType;


public interface DreamTokenTypes {
    IElementType KEYWORD = new DreamElementType("KEYWORD");
    IElementType NUMBER = new DreamElementType("NUMBER");
    IElementType STRING = new DreamElementType("STRING");
    IElementType LINE_COMMENT = new DreamElementType("LINE_COMMENT");
    IElementType BLOCK_COMMENT = new DreamElementType("BLOCK_COMMENT");
    IElementType COMMA = new DreamElementType("COMMA");
}
