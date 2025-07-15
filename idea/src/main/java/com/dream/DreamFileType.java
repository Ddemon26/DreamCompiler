package com.dream;

import com.intellij.openapi.fileTypes.LanguageFileType;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;
import icons.MyIcons;

public class DreamFileType extends LanguageFileType {
    public static final DreamFileType INSTANCE = new DreamFileType();

    private DreamFileType() {
        super(DreamLanguage.INSTANCE);
    }

    @NotNull
    @Override
    public String getName() {
        return "Dream file";
    }

    @NotNull
    @Override
    public String getDescription() {
        return "Dream language file";
    }

    @NotNull
    @Override
    public String getDefaultExtension() {
        return "dr";
    }

    @Nullable
    @Override
    public Icon getIcon() {
        return MyIcons.FILE;
    }
}
