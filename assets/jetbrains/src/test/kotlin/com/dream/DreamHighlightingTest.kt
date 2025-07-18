import com.intellij.testFramework.fixtures.BasePlatformTestCase
import com.intellij.openapi.fileTypes.SyntaxHighlighterFactory
import com.dream.DreamLanguage

class DreamHighlightingTest : BasePlatformTestCase() {
    fun testTokenAttributesNotEmpty() {
        val text = """
        /** doc */
        // line
        /* block */
        func main() {
            let x = 1 + 2;
            Console.WriteLine("hello");
        }
        """
        val file = myFixture.configureByText("sample.dr", text)
        val highlighter = SyntaxHighlighterFactory.getSyntaxHighlighter(DreamLanguage, project, file.virtualFile)
        val lexer = highlighter.highlightingLexer
        lexer.start(text)
        while (lexer.tokenType != null) {
            val type = lexer.tokenType!!
            if (type != com.intellij.psi.TokenType.WHITE_SPACE &&
                type != com.intellij.psi.TokenType.BAD_CHARACTER &&
                type != com.dream.DreamTokenTypes.IDENTIFIER) {
                val attrs = highlighter.getTokenHighlights(type)
                assertTrue("Token $type should have attributes", attrs.isNotEmpty())
            }
            lexer.advance()
        }
    }

    fun testKeywordsAndNumbersRecognized() {
        val text = "if let x = 123" 
        val file = myFixture.configureByText("sample.dr", text)
        val highlighter = SyntaxHighlighterFactory.getSyntaxHighlighter(DreamLanguage, project, file.virtualFile)
        val lexer = highlighter.highlightingLexer
        val tokens = mutableListOf<com.intellij.psi.tree.IElementType>()
        lexer.start(text)
        while (lexer.tokenType != null) {
            tokens.add(lexer.tokenType!!)
            lexer.advance()
        }
        assertTrue(tokens.contains(com.dream.DreamTokenTypes.KEYWORD))
        assertTrue(tokens.contains(com.dream.DreamTokenTypes.NUMBER))
    }

    fun testFunctionTokenRecognized() {
        val text = "func int Add() {}"
        val file = myFixture.configureByText("sample.dr", text)
        val highlighter = SyntaxHighlighterFactory.getSyntaxHighlighter(DreamLanguage, project, file.virtualFile)
        val lexer = highlighter.highlightingLexer
        var found = false
        lexer.start(text)
        while (lexer.tokenType != null) {
            if (lexer.tokenType == com.dream.DreamTokenTypes.FUNCTION) {
                found = true
            }
            lexer.advance()
        }
        assertTrue(found)
    }
}
