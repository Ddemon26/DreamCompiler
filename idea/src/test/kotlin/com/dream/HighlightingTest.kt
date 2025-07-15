package com.dream

import com.intellij.openapi.fileTypes.SyntaxHighlighterFactory
import com.intellij.testFramework.fixtures.BasePlatformTestCase
import org.junit.Assert.assertTrue

class HighlightingTest : BasePlatformTestCase() {
    fun testTokensHaveAttributes() {
        val text = "// comment\nint x = 42, y = 0\nConsole.WriteLine(\"hi\")"
        val virtualFile = myFixture.configureByText("sample.dr", text).virtualFile
        val highlighter = SyntaxHighlighterFactory.getSyntaxHighlighter(DreamFileType, project, virtualFile)!!
        val lexer = highlighter.highlightingLexer
        lexer.start(text)
        var hasColors = false
        while (lexer.tokenType != null) {
            if (lexer.tokenType !in setOf(com.intellij.psi.TokenType.WHITE_SPACE)) {
                val keys = highlighter.getTokenHighlights(lexer.tokenType)
                if (keys.isNotEmpty()) { hasColors = true; break }
            }
            lexer.advance()
        }
        assertTrue("Expected at least one highlighted token", hasColors)
    }
}
