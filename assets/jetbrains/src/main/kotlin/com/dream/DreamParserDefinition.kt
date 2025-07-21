package com.dream

import com.dream.parser.DreamParser
import com.dream.psi.DreamFile
import com.dream.psi.DreamTypes
import com.intellij.lang.ASTNode
import com.intellij.lang.ParserDefinition
import com.intellij.lang.PsiParser
import com.intellij.lexer.Lexer
import com.intellij.openapi.project.Project
import com.intellij.psi.FileViewProvider
import com.intellij.psi.PsiElement
import com.intellij.psi.PsiFile
import com.intellij.psi.tree.IFileElementType
import com.intellij.psi.tree.TokenSet

class DreamParserDefinition : ParserDefinition {

    companion object {
        val WHITE_SPACES = TokenSet.create(DreamTypes.WHITE_SPACE)
        val COMMENTS = TokenSet.create(DreamTypes.LINE_COMMENT, DreamTypes.BLOCK_COMMENT)
        val STRINGS = TokenSet.create(DreamTypes.STRING_LITERAL)
        val FILE = IFileElementType(DreamLanguage)
    }

    override fun createLexer(project: Project?): Lexer {
        return DreamLexerAdapter()
    }

    override fun createParser(project: Project?): PsiParser {
        return DreamParser()
    }

    override fun getFileNodeType(): IFileElementType {
        return FILE
    }

    override fun getCommentTokens(): TokenSet {
        return COMMENTS
    }

    override fun getStringLiteralElements(): TokenSet {
        return STRINGS
    }

    override fun getWhitespaceTokens(): TokenSet {
        return WHITE_SPACES
    }

    override fun createElement(node: ASTNode?): PsiElement {
        return DreamTypes.Factory.createElement(node)
    }

    override fun createFile(viewProvider: FileViewProvider): PsiFile {
        return DreamFile(viewProvider)
    }
}