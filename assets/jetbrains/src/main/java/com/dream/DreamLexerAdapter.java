package com.dream;

import com.intellij.lexer.FlexAdapter;

public class DreamLexerAdapter extends FlexAdapter {
    public DreamLexerAdapter() {
        super(new DreamLexer(null));
    }
}
