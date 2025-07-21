package com.dream

import com.intellij.testFramework.fixtures.BasePlatformTestCase
import com.intellij.codeInsight.completion.CompletionType

class DreamCompletionTest : BasePlatformTestCase() {
    
    /**
     * Test basic keyword completion
     */
    fun testKeywordCompletion() {
        val text = """
        func main() {
            i<caret>
        }
        """
        
        myFixture.configureByText("test.dr", text)
        myFixture.complete(CompletionType.BASIC)
        
        val completions = myFixture.lookupElementStrings
        
        assertNotNull("Completions should not be null", completions)
        assertTrue("Should contain 'if' keyword", completions?.contains("if") == true)
        assertTrue("Should contain 'int' type", completions?.contains("int") == true)
        assertTrue("Should contain 'import' keyword", completions?.contains("import") == true)
    }
    
    /**
     * Test Console function completion
     */
    fun testConsoleFunctionCompletion() {
        val text = """
        func main() {
            Console.<caret>
        }
        """
        
        myFixture.configureByText("test.dr", text)
        myFixture.complete(CompletionType.BASIC)
        
        val completions = myFixture.lookupElementStrings
        
        assertNotNull("Completions should not be null", completions)
        assertTrue("Should contain 'WriteLine'", completions?.contains("WriteLine") == true)
        assertTrue("Should contain 'Write'", completions?.contains("Write") == true)
        assertTrue("Should contain 'ReadLine'", completions?.contains("ReadLine") == true)
    }
    
    /**
     * Test type keyword completion in variable declarations
     */
    fun testTypeCompletionInVariableDeclaration() {
        val text = """
        func test() {
            s<caret> x;
        }
        """
        
        myFixture.configureByText("test.dr", text)
        myFixture.complete(CompletionType.BASIC)
        
        val completions = myFixture.lookupElementStrings
        
        assertNotNull("Completions should not be null", completions)
        assertTrue("Should contain 'string' type", completions?.contains("string") == true)
        assertTrue("Should contain 'struct' keyword", completions?.contains("struct") == true)
        assertTrue("Should contain 'static' keyword", completions?.contains("static") == true)
    }
    
    /**
     * Test completion in class context
     */
    fun testClassContextCompletion() {
        val text = """
        class TestClass {
            f<caret>
        }
        """
        
        myFixture.configureByText("test.dr", text)
        myFixture.complete(CompletionType.BASIC)
        
        val completions = myFixture.lookupElementStrings
        
        assertNotNull("Completions should not be null", completions)
        assertTrue("Should contain 'func' keyword", completions?.contains("func") == true)
        assertTrue("Should contain 'float' type", completions?.contains("float") == true)
    }
    
    /**
     * Test completion with control flow keywords
     */
    fun testControlFlowCompletion() {
        val text = """
        func test(bool condition) {
            if (condition) {
                r<caret>
            }
        }
        """
        
        myFixture.configureByText("test.dr", text)
        myFixture.complete(CompletionType.BASIC)
        
        val completions = myFixture.lookupElementStrings
        
        assertNotNull("Completions should not be null", completions)
        assertTrue("Should contain 'return' keyword", completions?.contains("return") == true)
    }
    
    /**
     * Test async/await completion
     */
    fun testAsyncCompletionKeywords() {
        val text = """
        a<caret> func test() {
        }
        """
        
        myFixture.configureByText("test.dr", text)
        myFixture.complete(CompletionType.BASIC)
        
        val completions = myFixture.lookupElementStrings
        
        assertNotNull("Completions should not be null", completions)
        assertTrue("Should contain 'async' keyword", completions?.contains("async") == true)
        assertTrue("Should contain 'await' keyword", completions?.contains("await") == true)
    }
    
    /**
     * Test module system keywords completion
     */
    fun testModuleSystemCompletion() {
        val text = """
        m<caret>
        """
        
        myFixture.configureByText("test.dr", text)
        myFixture.complete(CompletionType.BASIC)
        
        val completions = myFixture.lookupElementStrings
        
        assertNotNull("Completions should not be null", completions)
        assertTrue("Should contain 'module' keyword", completions?.contains("module") == true)
    }
    
    /**
     * Test import completion
     */
    fun testImportCompletion() {
        val text = """
        module test;
        i<caret>
        """
        
        myFixture.configureByText("test.dr", text)
        myFixture.complete(CompletionType.BASIC)
        
        val completions = myFixture.lookupElementStrings
        
        assertNotNull("Completions should not be null", completions)
        assertTrue("Should contain 'import' keyword", completions?.contains("import") == true)
    }
    
    /**
     * Test export completion
     */
    fun testExportCompletion() {
        val text = """
        module test;
        
        e<caret> class TestClass {
        }
        """
        
        myFixture.configureByText("test.dr", text)
        myFixture.complete(CompletionType.BASIC)
        
        val completions = myFixture.lookupElementStrings
        
        assertNotNull("Completions should not be null", completions)
        assertTrue("Should contain 'export' keyword", completions?.contains("export") == true)
    }
}