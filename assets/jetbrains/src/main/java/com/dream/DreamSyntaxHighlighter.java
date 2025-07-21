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
    public static final TextAttributesKey KEYWORDTYPE = TextAttributesKey.createTextAttributesKey("DREAM_KEYWORDTYPE", DefaultLanguageHighlighterColors.KEYWORD);
    public static final TextAttributesKey KEYWORDOTHER = TextAttributesKey.createTextAttributesKey("DREAM_KEYWORDOTHER", DefaultLanguageHighlighterColors.KEYWORD);
    public static final TextAttributesKey KEYWORDLITERAL = TextAttributesKey.createTextAttributesKey("DREAM_KEYWORDLITERAL", DefaultLanguageHighlighterColors.KEYWORD);
    public static final TextAttributesKey NUMBER = TextAttributesKey.createTextAttributesKey("DREAM_NUMBER", DefaultLanguageHighlighterColors.NUMBER);
    public static final TextAttributesKey STRING = TextAttributesKey.createTextAttributesKey("DREAM_STRING", DefaultLanguageHighlighterColors.STRING);
    public static final TextAttributesKey CHAR = TextAttributesKey.createTextAttributesKey("DREAM_CHAR", DefaultLanguageHighlighterColors.STRING);
    public static final TextAttributesKey COMMENT = TextAttributesKey.createTextAttributesKey("DREAM_COMMENT", DefaultLanguageHighlighterColors.LINE_COMMENT);
    public static final TextAttributesKey CONSOLEFUNCTION = TextAttributesKey.createTextAttributesKey("DREAM_CONSOLEFUNCTION", DefaultLanguageHighlighterColors.PREDEFINED_SYMBOL);
    public static final TextAttributesKey OPERATORARITHMETIC = TextAttributesKey.createTextAttributesKey("DREAM_OPERATORARITHMETIC", DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey OPERATORBITWISE = TextAttributesKey.createTextAttributesKey("DREAM_OPERATORBITWISE", DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey OPERATORCOMPARISON = TextAttributesKey.createTextAttributesKey("DREAM_OPERATORCOMPARISON", DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey OPERATORLOGICAL = TextAttributesKey.createTextAttributesKey("DREAM_OPERATORLOGICAL", DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey OPERATORASSIGNMENT = TextAttributesKey.createTextAttributesKey("DREAM_OPERATORASSIGNMENT", DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey OPERATORCONDITIONAL = TextAttributesKey.createTextAttributesKey("DREAM_OPERATORCONDITIONAL", DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey OPERATORARROW = TextAttributesKey.createTextAttributesKey("DREAM_OPERATORARROW", DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey OPERATOR = TextAttributesKey.createTextAttributesKey("DREAM_OPERATOR", DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey SEMICOLON = TextAttributesKey.createTextAttributesKey("DREAM_SEMICOLON", DefaultLanguageHighlighterColors.SEMICOLON);
    public static final TextAttributesKey COMMA = TextAttributesKey.createTextAttributesKey("DREAM_COMMA", DefaultLanguageHighlighterColors.COMMA);
    public static final TextAttributesKey DOT = TextAttributesKey.createTextAttributesKey("DREAM_DOT", DefaultLanguageHighlighterColors.DOT);
    public static final TextAttributesKey PAREN = TextAttributesKey.createTextAttributesKey("DREAM_PAREN", DefaultLanguageHighlighterColors.PARENTHESES);
    public static final TextAttributesKey BRACE = TextAttributesKey.createTextAttributesKey("DREAM_BRACE", DefaultLanguageHighlighterColors.BRACES);
    public static final TextAttributesKey BRACKET = TextAttributesKey.createTextAttributesKey("DREAM_BRACKET", DefaultLanguageHighlighterColors.BRACKETS);
    public static final TextAttributesKey IDENTIFIER = TextAttributesKey.createTextAttributesKey("DREAM_IDENTIFIER", DefaultLanguageHighlighterColors.IDENTIFIER);
    public static final TextAttributesKey FUNCTION_DECLARATION = TextAttributesKey.createTextAttributesKey("DREAM_FUNCTION_DECLARATION", DefaultLanguageHighlighterColors.FUNCTION_DECLARATION);
    public static final TextAttributesKey CLASS_NAME = TextAttributesKey.createTextAttributesKey("DREAM_CLASS_NAME", DefaultLanguageHighlighterColors.CLASS_NAME);
    public static final TextAttributesKey LOCAL_VARIABLE = TextAttributesKey.createTextAttributesKey("DREAM_LOCAL_VARIABLE", DefaultLanguageHighlighterColors.LOCAL_VARIABLE);

    private static final TextAttributesKey[] KEYWORD_KEYS = pack(KEYWORD);
    private static final TextAttributesKey[] KEYWORDTYPE_KEYS = pack(KEYWORDTYPE);
    private static final TextAttributesKey[] KEYWORDOTHER_KEYS = pack(KEYWORDOTHER);
    private static final TextAttributesKey[] KEYWORDLITERAL_KEYS = pack(KEYWORDLITERAL);
    private static final TextAttributesKey[] NUMBER_KEYS = pack(NUMBER);
    private static final TextAttributesKey[] STRING_KEYS = pack(STRING);
    private static final TextAttributesKey[] CHAR_KEYS = pack(CHAR);
    private static final TextAttributesKey[] COMMENT_KEYS = pack(COMMENT);
    private static final TextAttributesKey[] CONSOLEFUNCTION_KEYS = pack(CONSOLEFUNCTION);
    private static final TextAttributesKey[] OPERATORARITHMETIC_KEYS = pack(OPERATORARITHMETIC);
    private static final TextAttributesKey[] OPERATORBITWISE_KEYS = pack(OPERATORBITWISE);
    private static final TextAttributesKey[] OPERATORCOMPARISON_KEYS = pack(OPERATORCOMPARISON);
    private static final TextAttributesKey[] OPERATORLOGICAL_KEYS = pack(OPERATORLOGICAL);
    private static final TextAttributesKey[] OPERATORASSIGNMENT_KEYS = pack(OPERATORASSIGNMENT);
    private static final TextAttributesKey[] OPERATORCONDITIONAL_KEYS = pack(OPERATORCONDITIONAL);
    private static final TextAttributesKey[] OPERATORARROW_KEYS = pack(OPERATORARROW);
    private static final TextAttributesKey[] SEMICOLON_KEYS = pack(SEMICOLON);
    private static final TextAttributesKey[] COMMA_KEYS = pack(COMMA);
    private static final TextAttributesKey[] DOT_KEYS = pack(DOT);
    private static final TextAttributesKey[] PAREN_KEYS = pack(PAREN);
    private static final TextAttributesKey[] BRACE_KEYS = pack(BRACE);
    private static final TextAttributesKey[] BRACKET_KEYS = pack(BRACKET);
    private static final TextAttributesKey[] IDENTIFIER_KEYS = pack(IDENTIFIER);
    private static final TextAttributesKey[] FUNCTION_DECLARATION_KEYS = pack(FUNCTION_DECLARATION);
    private static final TextAttributesKey[] CLASS_NAME_KEYS = pack(CLASS_NAME);
    private static final TextAttributesKey[] LOCAL_VARIABLE_KEYS = pack(LOCAL_VARIABLE);

    @NotNull
    @Override
    public TextAttributesKey[] getTokenHighlights(IElementType tokenType) {
        if (tokenType == DreamTokenTypes.KEYWORD) return KEYWORD_KEYS;
        if (tokenType == DreamTokenTypes.KEYWORDTYPE) return KEYWORDTYPE_KEYS;
        if (tokenType == DreamTokenTypes.KEYWORDOTHER) return KEYWORDOTHER_KEYS;
        if (tokenType == DreamTokenTypes.KEYWORDLITERAL) return KEYWORDLITERAL_KEYS;
        if (tokenType == DreamTokenTypes.NUMBER) return NUMBER_KEYS;
        if (tokenType == DreamTokenTypes.STRING) return STRING_KEYS;
        if (tokenType == DreamTokenTypes.CHAR) return CHAR_KEYS;
        if (tokenType == DreamTokenTypes.COMMENT ||
            tokenType == DreamTokenTypes.COMMENTBLOCK ||
            tokenType == DreamTokenTypes.COMMENTDOC) return COMMENT_KEYS;
        if (tokenType == DreamTokenTypes.CONSOLEFUNCTION) return CONSOLEFUNCTION_KEYS;
        if (tokenType == DreamTokenTypes.OPERATORARITHMETIC) return OPERATORARITHMETIC_KEYS;
        if (tokenType == DreamTokenTypes.OPERATORBITWISE) return OPERATORBITWISE_KEYS;
        if (tokenType == DreamTokenTypes.OPERATORCOMPARISON) return OPERATORCOMPARISON_KEYS;
        if (tokenType == DreamTokenTypes.OPERATORLOGICAL) return OPERATORLOGICAL_KEYS;
        if (tokenType == DreamTokenTypes.OPERATORASSIGNMENT) return OPERATORASSIGNMENT_KEYS;
        if (tokenType == DreamTokenTypes.OPERATORCONDITIONAL) return OPERATORCONDITIONAL_KEYS;
        if (tokenType == DreamTokenTypes.OPERATORARROW) return OPERATORARROW_KEYS;
        if (tokenType == DreamTokenTypes.SEMICOLON) return SEMICOLON_KEYS;
        if (tokenType == DreamTokenTypes.COMMA) return COMMA_KEYS;
        if (tokenType == DreamTokenTypes.DOT) return DOT_KEYS;
        if (tokenType == DreamTokenTypes.PAREN) return PAREN_KEYS;
        if (tokenType == DreamTokenTypes.BRACE) return BRACE_KEYS;
        if (tokenType == DreamTokenTypes.BRACKET) return BRACKET_KEYS;
        if (tokenType == DreamTokenTypes.IDENTIFIER) return IDENTIFIER_KEYS;
        return TextAttributesKey.EMPTY_ARRAY;
    }
}
