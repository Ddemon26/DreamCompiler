package com.dream;

import com.intellij.openapi.editor.colors.TextAttributesKey;
import com.intellij.openapi.fileTypes.SyntaxHighlighter;
import com.intellij.openapi.options.colors.AttributesDescriptor;
import com.intellij.openapi.options.colors.ColorDescriptor;
import com.intellij.openapi.options.colors.ColorSettingsPage;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;
import java.util.Map;

public class DreamColorSettingsPage implements ColorSettingsPage {
    private static final AttributesDescriptor[] DESCRIPTORS = new AttributesDescriptor[]{
            new AttributesDescriptor("Keyword", DreamSyntaxHighlighter.KEYWORD_KEYS),
            new AttributesDescriptor("Number", DreamSyntaxHighlighter.NUMBER_KEYS),
            new AttributesDescriptor("String", DreamSyntaxHighlighter.STRING_KEYS),
            new AttributesDescriptor("Comment", DreamSyntaxHighlighter.COMMENT_KEYS)
    };

    @Override
    public Icon getIcon() {
        return null;
    }

    @Override
    public SyntaxHighlighter getHighlighter() {
        return new DreamSyntaxHighlighter();
    }

    @NotNull
    @Override
    public String getDemoText() {
        return "// greeting\n" +
                "/* example block */\n" +
                "string msg = \"hi\";\n" +
                "int x = 5, y = 10;\n" +
                "if (true) {\n    Console.WriteLine(msg, x + y);\n}";
    }

    @Nullable
    @Override
    public Map<String, TextAttributesKey> getAdditionalHighlightingTagToDescriptorMap() {
        return null;
    }

    @Override
    public AttributesDescriptor[] getAttributeDescriptors() {
        return DESCRIPTORS;
    }

    @Override
    public ColorDescriptor[] getColorDescriptors() {
        return ColorDescriptor.EMPTY_ARRAY;
    }

    @NotNull
    @Override
    public String getDisplayName() {
        return "Dream";
    }
}
