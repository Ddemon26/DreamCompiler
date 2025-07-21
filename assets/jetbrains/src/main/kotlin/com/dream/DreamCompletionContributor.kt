package com.dream

import com.intellij.codeInsight.completion.*
import com.intellij.codeInsight.lookup.LookupElementBuilder
import com.intellij.patterns.PlatformPatterns
import com.intellij.psi.PsiElement
import com.intellij.util.ProcessingContext

class DreamCompletionContributor : CompletionContributor() {
    
    private val allKeywords = listOf(
        // Control keywords
        "if", "else", "while", "for", "do", "break", "continue", "return", 
        "switch", "case", "default", "try", "catch", "finally", "throw",
        // Type keywords
        "int", "string", "bool", "float", "char", "void",
        // Modifier keywords
        "class", "struct", "static", "new", "func", "var", "let", "base", 
        "using", "import", "module", "export", "async", "await", "Task", "TaskResult",
        // Literal keywords
        "true", "false"
    )
    
    private val consoleFunctions = listOf(
        "WriteLine", "Write", "ReadLine"
    )
    
    init {
        extend(
            CompletionType.BASIC,
            PlatformPatterns.psiElement().inFile(PlatformPatterns.psiFile().withName(PlatformPatterns.string().endsWith(".dr"))),
            object : CompletionProvider<CompletionParameters>() {
                override fun addCompletions(
                    parameters: CompletionParameters,
                    context: ProcessingContext,
                    result: CompletionResultSet
                ) {
                    // Check if we're in a Dream file
                    val file = parameters.originalFile
                    if (!file.name.endsWith(".dr")) {
                        return
                    }
                    
                    // Add all keywords
                    allKeywords.forEach { keyword ->
                        result.addElement(LookupElementBuilder.create(keyword)
                            .withTypeText("keyword")
                            .withBoldness(true))
                    }
                    
                    // Add Console functions
                    consoleFunctions.forEach { func ->
                        result.addElement(LookupElementBuilder.create(func)
                            .withTypeText("Console function"))
                    }
                    
                    // Add full Console.Method completions
                    result.addElement(LookupElementBuilder.create("Console.Write")
                        .withTypeText("Console method"))
                    result.addElement(LookupElementBuilder.create("Console.WriteLine")
                        .withTypeText("Console method"))
                    result.addElement(LookupElementBuilder.create("Console.ReadLine")
                        .withTypeText("Console method"))
                }
            }
        )
    }
}
