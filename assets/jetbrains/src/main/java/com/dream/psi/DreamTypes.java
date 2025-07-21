package com.dream.psi;

import com.intellij.psi.tree.IElementType;
import com.intellij.psi.tree.TokenSet;

public interface DreamTypes {
    IElementType WHITE_SPACE = new DreamTokenType("WHITE_SPACE");
    IElementType LINE_COMMENT = new DreamTokenType("LINE_COMMENT");
    IElementType BLOCK_COMMENT = new DreamTokenType("BLOCK_COMMENT");
    IElementType STRING_LITERAL = new DreamTokenType("STRING_LITERAL");
    
    // Keywords
    IElementType BREAK = new DreamTokenType("BREAK");
    IElementType BOOL = new DreamTokenType("BOOL");
    IElementType CASE = new DreamTokenType("CASE");
    IElementType CHAR = new DreamTokenType("CHAR");
    IElementType CLASS = new DreamTokenType("CLASS");
    IElementType STRUCT = new DreamTokenType("STRUCT");
    IElementType CONTINUE = new DreamTokenType("CONTINUE");
    IElementType DEFAULT = new DreamTokenType("DEFAULT");
    IElementType DO = new DreamTokenType("DO");
    IElementType ELSE = new DreamTokenType("ELSE");
    IElementType FALSE = new DreamTokenType("FALSE");
    IElementType FLOAT = new DreamTokenType("FLOAT");
    IElementType FOR = new DreamTokenType("FOR");
    IElementType FUNC = new DreamTokenType("FUNC");
    IElementType IF = new DreamTokenType("IF");
    IElementType INT = new DreamTokenType("INT");
    IElementType RETURN = new DreamTokenType("RETURN");
    IElementType STRING = new DreamTokenType("STRING");
    IElementType SWITCH = new DreamTokenType("SWITCH");
    IElementType TRUE = new DreamTokenType("TRUE");
    IElementType WHILE = new DreamTokenType("WHILE");
    IElementType MODULE = new DreamTokenType("MODULE");
    IElementType IMPORT = new DreamTokenType("IMPORT");
    IElementType EXPORT = new DreamTokenType("EXPORT");
    IElementType CONSOLE = new DreamTokenType("CONSOLE");
    
    // Operators and punctuation
    IElementType PLUS = new DreamTokenType("PLUS");
    IElementType MINUS = new DreamTokenType("MINUS");
    IElementType MULTIPLY = new DreamTokenType("MULTIPLY");
    IElementType DIVIDE = new DreamTokenType("DIVIDE");
    IElementType MODULO = new DreamTokenType("MODULO");
    IElementType ASSIGN = new DreamTokenType("ASSIGN");
    IElementType EQUAL = new DreamTokenType("EQUAL");
    IElementType NOT_EQUAL = new DreamTokenType("NOT_EQUAL");
    IElementType LESS_THAN = new DreamTokenType("LESS_THAN");
    IElementType LESS_EQUAL = new DreamTokenType("LESS_EQUAL");
    IElementType GREATER_THAN = new DreamTokenType("GREATER_THAN");
    IElementType GREATER_EQUAL = new DreamTokenType("GREATER_EQUAL");
    IElementType LOGICAL_AND = new DreamTokenType("LOGICAL_AND");
    IElementType LOGICAL_OR = new DreamTokenType("LOGICAL_OR");
    IElementType LOGICAL_NOT = new DreamTokenType("LOGICAL_NOT");
    IElementType SEMICOLON = new DreamTokenType("SEMICOLON");
    IElementType COMMA = new DreamTokenType("COMMA");
    IElementType DOT = new DreamTokenType("DOT");
    IElementType LEFT_PAREN = new DreamTokenType("LEFT_PAREN");
    IElementType RIGHT_PAREN = new DreamTokenType("RIGHT_PAREN");
    IElementType LEFT_BRACE = new DreamTokenType("LEFT_BRACE");
    IElementType RIGHT_BRACE = new DreamTokenType("RIGHT_BRACE");
    IElementType LEFT_BRACKET = new DreamTokenType("LEFT_BRACKET");
    IElementType RIGHT_BRACKET = new DreamTokenType("RIGHT_BRACKET");
    
    // Literals and identifiers
    IElementType NUMBER = new DreamTokenType("NUMBER");
    IElementType CHAR_LITERAL = new DreamTokenType("CHAR_LITERAL");
    IElementType IDENTIFIER = new DreamTokenType("IDENTIFIER");
    
    // Element types
    IElementType COMPILATION_UNIT = new DreamElementType("COMPILATION_UNIT");
    IElementType FUNCTION_DECLARATION = new DreamElementType("FUNCTION_DECLARATION");
    IElementType CLASS_DECLARATION = new DreamElementType("CLASS_DECLARATION");
    IElementType VARIABLE_DECLARATION = new DreamElementType("VARIABLE_DECLARATION");
    IElementType STATEMENT = new DreamElementType("STATEMENT");
    IElementType EXPRESSION = new DreamElementType("EXPRESSION");
    
    TokenSet WHITE_SPACES = TokenSet.create(WHITE_SPACE);
    TokenSet COMMENTS = TokenSet.create(LINE_COMMENT, BLOCK_COMMENT);
    TokenSet STRINGS = TokenSet.create(STRING_LITERAL);
    
    class Factory {
        public static com.intellij.psi.PsiElement createElement(com.intellij.lang.ASTNode node) {
            // For now, return a basic wrapper
            return new com.intellij.extapi.psi.ASTWrapperPsiElement(node);
        }
    }
}