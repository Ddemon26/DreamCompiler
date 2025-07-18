package com.dream;

import com.intellij.psi.tree.IElementType;
import org.jetbrains.annotations.NonNls;
import org.jetbrains.annotations.NotNull;

public class DreamElementType extends IElementType {
    public DreamElementType(@NotNull @NonNls String debugName) {
        super(debugName, DreamLanguage.INSTANCE);
    }
}
