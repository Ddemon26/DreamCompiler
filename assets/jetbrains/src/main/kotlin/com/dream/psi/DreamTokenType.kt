package com.dream.psi

import com.dream.DreamLanguage
import com.intellij.psi.tree.IElementType

class DreamTokenType(debugName: String) : IElementType(debugName, DreamLanguage) {
    override fun toString(): String = "DreamTokenType." + super.toString()
}