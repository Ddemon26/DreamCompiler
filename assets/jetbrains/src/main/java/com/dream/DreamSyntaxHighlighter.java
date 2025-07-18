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
    public static final TextAttributesKey OPERATOR = TextAttributesKey.createTextAttributesKey("DREAM_OPERATOR", DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey SEMICOLON = TextAttributesKey.createTextAttributesKey("DREAM_SEMICOLON", DefaultLanguageHighlighterColors.SEMICOLON);
    public static final TextAttributesKey COMMA = TextAttributesKey.createTextAttributesKey("DREAM_COMMA", DefaultLanguageHighlighterColors.COMMA);
    public static final TextAttributesKey DOT = TextAttributesKey.createTextAttributesKey("DREAM_DOT", DefaultLanguageHighlighterColors.DOT);
    public static final TextAttributesKey PAREN = TextAttributesKey.createTextAttributesKey("DREAM_PAREN", DefaultLanguageHighlighterColors.PARENTHESES);
    public static final TextAttributesKey BRACE = TextAttributesKey.createTextAttributesKey("DREAM_BRACE", DefaultLanguageHighlighterColors.BRACES);
    public static final TextAttributesKey FUNCTION = TextAttributesKey.createTextAttributesKey("DREAM_FUNCTION", DefaultLanguageHighlighterColors.FUNCTION_DECLARATION);
    public static final TextAttributesKey BRACKET = TextAttributesKey.createTextAttributesKey("DREAM_BRACKET", DefaultLanguageHighlighterColors.BRACKETS);

    private static final TextAttributesKey[] KEYWORD_KEYS = pack(KEYWORD);
    private static final TextAttributesKey[] NUMBER_KEYS = pack(NUMBER);
    private static final TextAttributesKey[] STRING_KEYS = pack(STRING);
    private static final TextAttributesKey[] COMMENT_KEYS = pack(COMMENT);
    private static final TextAttributesKey[] OPERATOR_KEYS = pack(OPERATOR);
    private static final TextAttributesKey[] SEMICOLON_KEYS = pack(SEMICOLON);
    private static final TextAttributesKey[] COMMA_KEYS = pack(COMMA);
    private static final TextAttributesKey[] DOT_KEYS = pack(DOT);
    private static final TextAttributesKey[] PAREN_KEYS = pack(PAREN);
    private static final TextAttributesKey[] BRACE_KEYS = pack(BRACE);
    private static final TextAttributesKey[] FUNCTION_KEYS = pack(FUNCTION);
    private static final TextAttributesKey[] BRACKET_KEYS = pack(BRACKET);

    @NotNull
    @Override
    public TextAttributesKey[] getTokenHighlights(IElementType tokenType) {
        if (tokenType == DreamTokenTypes.KEYWORD) return KEYWORD_KEYS;
        if (tokenType == DreamTokenTypes.NUMBER) return NUMBER_KEYS;
        if (tokenType == DreamTokenTypes.STRING) return STRING_KEYS;
        if (tokenType == DreamTokenTypes.COMMENT ||
            tokenType == DreamTokenTypes.COMMENTBLOCK ||
            tokenType == DreamTokenTypes.COMMENTDOC) return COMMENT_KEYS;
        if (tokenType == DreamTokenTypes.OPERATOR) return OPERATOR_KEYS;
        if (tokenType == DreamTokenTypes.SEMICOLON) return SEMICOLON_KEYS;
        if (tokenType == DreamTokenTypes.COMMA) return COMMA_KEYS;
        if (tokenType == DreamTokenTypes.DOT) return DOT_KEYS;
        if (tokenType == DreamTokenTypes.PAREN) return PAREN_KEYS;
        if (tokenType == DreamTokenTypes.BRACE) return BRACE_KEYS;
        if (tokenType == DreamTokenTypes.FUNCTION) return FUNCTION_KEYS;
        if (tokenType == DreamTokenTypes.BRACKET) return BRACKET_KEYS;
        return TextAttributesKey.EMPTY_ARRAY;
    }
}
