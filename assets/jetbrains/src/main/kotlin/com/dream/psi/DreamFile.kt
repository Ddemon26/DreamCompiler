package com.dream.psi

import com.dream.DreamFileType
import com.dream.DreamLanguage
import com.intellij.extapi.psi.PsiFileBase
import com.intellij.openapi.fileTypes.FileType
import com.intellij.psi.FileViewProvider

class DreamFile(viewProvider: FileViewProvider) : PsiFileBase(viewProvider, DreamLanguage) {
    
    override fun getFileType(): FileType {
        return DreamFileType.INSTANCE
    }
    
    override fun toString(): String {
        return "Dream File"
    }
}