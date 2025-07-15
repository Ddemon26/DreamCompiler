package com.dream;

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

    public static final TextAttributesKey KEYWORD_KEYS = TextAttributesKey.createTextAttributesKey("DREAM_KEYWORD", DefaultLanguageHighlighterColors.KEYWORD);
    public static final TextAttributesKey NUMBER_KEYS = TextAttributesKey.createTextAttributesKey("DREAM_NUMBER", DefaultLanguageHighlighterColors.NUMBER);
    public static final TextAttributesKey STRING_KEYS = TextAttributesKey.createTextAttributesKey("DREAM_STRING", DefaultLanguageHighlighterColors.STRING);
    public static final TextAttributesKey COMMENT_KEYS = TextAttributesKey.createTextAttributesKey("DREAM_COMMENT", DefaultLanguageHighlighterColors.LINE_COMMENT);

    @NotNull
    @Override
    public TextAttributesKey[] getTokenHighlights(IElementType tokenType) {
        if (tokenType == DreamTokenTypes.KEYWORD) return pack(KEYWORD_KEYS);
        if (tokenType == DreamTokenTypes.NUMBER) return pack(NUMBER_KEYS);
        if (tokenType == DreamTokenTypes.STRING) return pack(STRING_KEYS);
        if (tokenType == DreamTokenTypes.LINE_COMMENT || tokenType == DreamTokenTypes.BLOCK_COMMENT) return pack(COMMENT_KEYS);
        return TextAttributesKey.EMPTY_ARRAY;
    }
}
