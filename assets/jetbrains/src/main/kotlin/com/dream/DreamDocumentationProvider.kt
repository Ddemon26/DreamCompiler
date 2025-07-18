package com.dream

import com.intellij.lang.documentation.AbstractDocumentationProvider
import com.intellij.psi.PsiElement

class DreamDocumentationProvider : AbstractDocumentationProvider() {
    override fun generateDoc(element: PsiElement?, originalElement: PsiElement?): String? {
        if (element == null) return null
        val text = element.text
        return when (text) {
            "Console.Write", "Console.WriteLine" -> "Outputs text to the console"
            else -> null
        }
    }
}
