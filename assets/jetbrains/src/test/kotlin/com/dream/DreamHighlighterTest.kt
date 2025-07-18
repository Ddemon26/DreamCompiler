import com.dream.DreamSyntaxHighlighter
import com.dream.DreamLanguage
import com.intellij.openapi.fileTypes.SyntaxHighlighterFactory
import com.intellij.testFramework.fixtures.BasePlatformTestCase

class DreamHighlighterTest : BasePlatformTestCase() {
    fun testTokensHaveAttributes() {
        val text = "if true { \"hi\" } // comment"
        val highlighter = SyntaxHighlighterFactory.getSyntaxHighlighter(DreamLanguage, project, null)
        val lexer = highlighter.highlightingLexer
        lexer.start(text)
        while (lexer.tokenType != null) {
            val type = lexer.tokenType!!
            if (type !== com.intellij.psi.TokenType.WHITE_SPACE &&
                type !== com.intellij.psi.TokenType.BAD_CHARACTER) {
                val keys = highlighter.getTokenHighlights(type)
                assertTrue("Token $type should have highlighting", keys.isNotEmpty())
            }
            lexer.advance()
        }
    }
}
