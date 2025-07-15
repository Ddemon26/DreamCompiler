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

    public static final TextAttributesKey KEYWORD = TextAttributesKey.createTextAttributesKey("DREAM_KEYWORD", DefaultLanguageHighlighterColors.KEYWORD);
    public static final TextAttributesKey NUMBER = TextAttributesKey.createTextAttributesKey("DREAM_NUMBER", DefaultLanguageHighlighterColors.NUMBER);
    public static final TextAttributesKey STRING = TextAttributesKey.createTextAttributesKey("DREAM_STRING", DefaultLanguageHighlighterColors.STRING);
    public static final TextAttributesKey COMMENT = TextAttributesKey.createTextAttributesKey("DREAM_COMMENT", DefaultLanguageHighlighterColors.LINE_COMMENT);

    private static final TextAttributesKey[] KEYWORD_KEYS = new TextAttributesKey[]{KEYWORD};
    private static final TextAttributesKey[] NUMBER_KEYS = new TextAttributesKey[]{NUMBER};
    private static final TextAttributesKey[] STRING_KEYS = new TextAttributesKey[]{STRING};
    private static final TextAttributesKey[] COMMENT_KEYS = new TextAttributesKey[]{COMMENT};

    @NotNull
    @Override
    public TextAttributesKey[] getTokenHighlights(IElementType tokenType) {
        if (tokenType == DreamTokenTypes.KEYWORD) return KEYWORD_KEYS;
        if (tokenType == DreamTokenTypes.NUMBER) return NUMBER_KEYS;
        if (tokenType == DreamTokenTypes.STRING) return STRING_KEYS;
        if (tokenType == DreamTokenTypes.LINE_COMMENT || tokenType == DreamTokenTypes.BLOCK_COMMENT) return COMMENT_KEYS;
        return TextAttributesKey.EMPTY_ARRAY;
    }
}
