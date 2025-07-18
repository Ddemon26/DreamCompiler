import com.intellij.testFramework.fixtures.BasePlatformTestCase
import com.dream.DreamSyntaxHighlighter

class HighlightingTest : BasePlatformTestCase() {
    fun testHighlightingAttributes() {
        val text = """// comment\nif true {\n  Console.WriteLine(\"hi\")\n}\n"""
        val highlighter = DreamSyntaxHighlighter()
        val lexer = highlighter.highlightingLexer
        lexer.start(text)
        while (lexer.tokenType != null) {
            val keys = highlighter.getTokenHighlights(lexer.tokenType!!)
            assertTrue("Attributes missing for ${lexer.tokenType}", keys.isNotEmpty())
            lexer.advance()
        }
    }
}
