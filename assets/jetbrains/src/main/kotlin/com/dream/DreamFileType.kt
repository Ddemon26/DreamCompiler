package com.dream

import com.intellij.openapi.fileTypes.LanguageFileType
import icons.MyIcons
import javax.swing.Icon

class DreamFileType : LanguageFileType(DreamLanguage) {
    
    companion object {
        @JvmField
        val INSTANCE = DreamFileType()
    }
    
    override fun getName() = "Dream file"
    override fun getDescription() = "Dream language file"
    override fun getDefaultExtension() = "dr"
    override fun getIcon(): Icon = MyIcons.FILE
}
