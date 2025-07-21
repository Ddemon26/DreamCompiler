import com.intellij.testFramework.fixtures.BasePlatformTestCase
import com.intellij.openapi.fileTypes.SyntaxHighlighterFactory
import com.intellij.psi.tree.IElementType
import com.dream.DreamLanguage
import com.dream.DreamTokenTypes

class DreamHighlightingTest : BasePlatformTestCase() {
    
    /**
     * Test that all recognized tokens have proper syntax highlighting attributes
     */
    fun testTokenAttributesNotEmpty() {
        val text = """
        // Test file for Dream syntax highlighting
        module test_module;
        
        import console_lib;
        export class TestClass;
        
        /** Documentation comment */
        // Line comment
        /* Block comment */
        
        func int calculate(int a, float b) {
            bool isValid = true;
            char delimiter = ';';
            string message = "Hello World";
            
            if (a > 0 && isValid) {
                return a + (int)b;
            } else {
                Console.WriteLine(message);
                return 0;
            }
        }
        """
        
        val file = myFixture.configureByText("sample.dr", text)
        val highlighter = SyntaxHighlighterFactory.getSyntaxHighlighter(DreamLanguage, project, file.virtualFile)
        val lexer = highlighter.highlightingLexer
        lexer.start(text)
        
        while (lexer.tokenType != null) {
            val type = lexer.tokenType!!
            if (type != com.intellij.psi.TokenType.WHITE_SPACE &&
                type != com.intellij.psi.TokenType.BAD_CHARACTER) {
                val attrs = highlighter.getTokenHighlights(type)
                assertTrue("Token $type should have attributes", attrs.isNotEmpty())
            }
            lexer.advance()
        }
    }

    /**
     * Test that all keyword categories are properly recognized
     */
    fun testKeywordCategorization() {
        val controlKeywords = "if else while for do break continue return switch case default try catch finally throw"
        val file1 = myFixture.configureByText("control.dr", controlKeywords)
        val tokens1 = extractTokens(controlKeywords)
        
        assertTrue("Control keywords should be recognized", 
                  tokens1.any { it == DreamTokenTypes.KEYWORD })
        
        val typeKeywords = "int string bool float char void"
        val tokens2 = extractTokens(typeKeywords)
        assertTrue("Type keywords should be recognized", 
                  tokens2.any { it == DreamTokenTypes.KEYWORDTYPE })
        
        val otherKeywords = "class struct static new func var let base using import module export async await Task TaskResult"
        val tokens3 = extractTokens(otherKeywords)
        assertTrue("Other keywords should be recognized", 
                  tokens3.any { it == DreamTokenTypes.KEYWORDOTHER })
        
        val literals = "true false"
        val tokens4 = extractTokens(literals)
        assertTrue("Literal keywords should be recognized", 
                  tokens4.any { it == DreamTokenTypes.KEYWORDLITERAL })
    }

    /**
     * Test numeric literal recognition
     */
    fun testNumericLiterals() {
        val numbers = listOf(
            "42",      // Integer
            "3.14",    // Float
            "0",       // Zero
            "1234567"  // Large number
        )
        
        for (num in numbers) {
            val tokens = extractTokens(num)
            assertTrue("Number '$num' should be recognized", 
                      tokens.contains(DreamTokenTypes.NUMBER))
        }
    }

    /**
     * Test string and character literal recognition
     */
    fun testStringLiterals() {
        val stringTests = listOf(
            "\"Hello World\"",
            "\"Escaped \\\"quotes\\\"\"",
            "\"Multi\\nline\"",
            "'a'",
            "'\\n'",
            "'\\\\'"
        )
        
        for (str in stringTests) {
            val tokens = extractTokens(str)
            val hasStringOrChar = tokens.contains(DreamTokenTypes.STRING) || 
                                  tokens.contains(DreamTokenTypes.CHAR)
            assertTrue("String/char '$str' should be recognized", hasStringOrChar)
        }
    }

    /**
     * Test comment recognition
     */
    fun testComments() {
        val comments = mapOf(
            "// Line comment" to DreamTokenTypes.COMMENT,
            "/* Block comment */" to DreamTokenTypes.COMMENTBLOCK,
            "/** Documentation comment */" to DreamTokenTypes.COMMENTDOC,
            "/// Doc comment" to DreamTokenTypes.COMMENTDOC
        )
        
        for ((comment, expectedType) in comments) {
            val tokens = extractTokens(comment)
            assertTrue("Comment '$comment' should be recognized as $expectedType", 
                      tokens.contains(expectedType))
        }
    }

    /**
     * Test operator categorization
     */
    fun testOperatorCategorization() {
        val operatorTests = mapOf(
            "+ - * / % ++ --" to DreamTokenTypes.OPERATORARITHMETIC,
            "& | ^ << >> ~" to DreamTokenTypes.OPERATORBITWISE,
            "< <= > >= == !=" to DreamTokenTypes.OPERATORCOMPARISON,
            "&& || !" to DreamTokenTypes.OPERATORLOGICAL,
            "=" to DreamTokenTypes.OPERATORASSIGNMENT,
            "? ?? ??=" to DreamTokenTypes.OPERATORCONDITIONAL,
            "-> =>" to DreamTokenTypes.OPERATORARROW
        )
        
        for ((ops, expectedType) in operatorTests) {
            val tokens = extractTokens(ops)
            assertTrue("Operators '$ops' should contain $expectedType", 
                      tokens.contains(expectedType))
        }
    }

    /**
     * Test Console function highlighting
     */
    fun testConsoleFunctions() {
        val consoleFunctions = listOf(
            "Console.WriteLine",
            "Console.Write", 
            "Console.ReadLine"
        )
        
        for (func in consoleFunctions) {
            val tokens = extractTokens(func)
            assertTrue("Console function '$func' should be recognized", 
                      tokens.contains(DreamTokenTypes.CONSOLEFUNCTION))
        }
    }

    /**
     * Test punctuation recognition
     */
    fun testPunctuation() {
        val punctuationTests = mapOf(
            ";" to DreamTokenTypes.SEMICOLON,
            "," to DreamTokenTypes.COMMA,
            "." to DreamTokenTypes.DOT,
            "(" to DreamTokenTypes.PAREN,
            ")" to DreamTokenTypes.PAREN,
            "{" to DreamTokenTypes.BRACE,
            "}" to DreamTokenTypes.BRACE,
            "[" to DreamTokenTypes.BRACKET,
            "]" to DreamTokenTypes.BRACKET
        )
        
        for ((punct, expectedType) in punctuationTests) {
            val tokens = extractTokens(punct)
            assertTrue("Punctuation '$punct' should be recognized as $expectedType", 
                      tokens.contains(expectedType))
        }
    }

    /**
     * Test complex Dream code example
     */
    fun testComplexExample() {
        val complexCode = """
        module math_utils;
        
        import string_utils;
        
        export class Calculator {
            float lastResult = 0.0f;
            
            func float add(float a, float b) {
                lastResult = a + b;
                return lastResult;
            }
        }
        
        async func Task<bool> validateAsync(int value) {
            try {
                bool isValid = value >= 0 && value <= 100;
                if (isValid) {
                    Console.WriteLine("Valid: " + value);
                } else {
                    throw "Invalid value";
                }
                return await Task.FromResult(isValid);
            } catch {
                Console.WriteLine("Error occurred");
                return false;
            }
        }
        """
        
        val tokens = extractTokens(complexCode)
        
        // Verify key token types are present
        assertTrue("Should contain control keywords", tokens.contains(DreamTokenTypes.KEYWORD))
        assertTrue("Should contain type keywords", tokens.contains(DreamTokenTypes.KEYWORDTYPE))
        assertTrue("Should contain other keywords", tokens.contains(DreamTokenTypes.KEYWORDOTHER))
        assertTrue("Should contain literal keywords", tokens.contains(DreamTokenTypes.KEYWORDLITERAL))
        assertTrue("Should contain numbers", tokens.contains(DreamTokenTypes.NUMBER))
        assertTrue("Should contain strings", tokens.contains(DreamTokenTypes.STRING))
        assertTrue("Should contain operators", 
                  tokens.any { it.toString().startsWith("OPERATOR") })
        assertTrue("Should contain Console functions", tokens.contains(DreamTokenTypes.CONSOLEFUNCTION))
    }

    /**
     * Helper method to extract tokens from text
     */
    private fun extractTokens(text: String): List<IElementType> {
        val file = myFixture.configureByText("test.dr", text)
        val highlighter = SyntaxHighlighterFactory.getSyntaxHighlighter(DreamLanguage, project, file.virtualFile)
        val lexer = highlighter.highlightingLexer
        val tokens = mutableListOf<IElementType>()
        
        lexer.start(text)
        while (lexer.tokenType != null) {
            tokens.add(lexer.tokenType!!)
            lexer.advance()
        }
        
        return tokens
    }
}
