package com.dream;

import com.intellij.lang.Language;

public class DreamLanguage extends Language {
    public static final DreamLanguage INSTANCE = new DreamLanguage();
    private DreamLanguage() {
        super("dream");
    }
}
