package com.dream;

import com.intellij.lexer.FlexAdapter;
import com.intellij.openapi.editor.DefaultLanguageHighlighterColors;
import com.intellij.lexer.Lexer;
import com.intellij.openapi.editor.colors.TextAttributesKey;
import com.intellij.openapi.fileTypes.SyntaxHighlighter;
import com.intellij.openapi.fileTypes.SyntaxHighlighterBase;
import com.intellij.psi.tree.IElementType;
import org.jetbrains.annotations.NotNull;


public class DreamSyntaxHighlighter extends SyntaxHighlighterBase {
    private final Lexer lexer = new DreamLexerAdapter();

    @NotNull
    @Override
    public Lexer getHighlightingLexer() {
        return lexer;
    }

    private static final TextAttributesKey KEYWORD = TextAttributesKey.createTextAttributesKey("DREAM_KEYWORD", DefaultLanguageHighlighterColors.KEYWORD);
    private static final TextAttributesKey NUMBER = TextAttributesKey.createTextAttributesKey("DREAM_NUMBER", DefaultLanguageHighlighterColors.NUMBER);
    private static final TextAttributesKey STRING = TextAttributesKey.createTextAttributesKey("DREAM_STRING", DefaultLanguageHighlighterColors.STRING);
    private static final TextAttributesKey COMMENT = TextAttributesKey.createTextAttributesKey("DREAM_COMMENT", DefaultLanguageHighlighterColors.LINE_COMMENT);

    @NotNull
    @Override
    public TextAttributesKey[] getTokenHighlights(IElementType tokenType) {
        if (tokenType == DreamTokenTypes.KEYWORD) return pack(KEYWORD);
        if (tokenType == DreamTokenTypes.NUMBER) return pack(NUMBER);
        if (tokenType == DreamTokenTypes.STRING) return pack(STRING);
        if (tokenType == DreamTokenTypes.COMMENT || tokenType == DreamTokenTypes.COMMENTBLOCK) return pack(COMMENT);
        return TextAttributesKey.EMPTY_ARRAY;
    }
}
