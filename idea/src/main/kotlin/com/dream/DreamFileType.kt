package com.dream

import com.intellij.openapi.fileTypes.LanguageFileType
import icons.MyIcons
import javax.swing.Icon

object DreamFileType : LanguageFileType(DreamLanguage) {
    override fun getName() = "Dream file"
    override fun getDescription() = "Dream language file"
    override fun getDefaultExtension() = "dr"
    override fun getIcon(): Icon = MyIcons.FILE
}
