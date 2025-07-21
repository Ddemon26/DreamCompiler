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
            new AttributesDescriptor("Keyword", DreamSyntaxHighlighter.KEYWORD),
            new AttributesDescriptor("Number", DreamSyntaxHighlighter.NUMBER),
            new AttributesDescriptor("String", DreamSyntaxHighlighter.STRING),
            new AttributesDescriptor("Comment", DreamSyntaxHighlighter.COMMENT),
            new AttributesDescriptor("Operator", DreamSyntaxHighlighter.OPERATOR),
            new AttributesDescriptor("Semicolon", DreamSyntaxHighlighter.SEMICOLON),
            new AttributesDescriptor("Comma", DreamSyntaxHighlighter.COMMA),
            new AttributesDescriptor("Dot", DreamSyntaxHighlighter.DOT),
            new AttributesDescriptor("Parentheses", DreamSyntaxHighlighter.PAREN),
            new AttributesDescriptor("Braces", DreamSyntaxHighlighter.BRACE),
            new AttributesDescriptor("Brackets", DreamSyntaxHighlighter.BRACKET),
            new AttributesDescriptor("Function Name", DreamSyntaxHighlighter.FUNCTION_NAME),
            new AttributesDescriptor("Function Call", DreamSyntaxHighlighter.FUNCTION_CALL),
            new AttributesDescriptor("Parameter", DreamSyntaxHighlighter.PARAMETER)
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
        return """
/** Documentation comment */
// This is a single-line comment.
/*
  This is a
  multi-line block comment.
*/
func int calculateSum(int a, int b) {
    // Using keywords, numbers, and operators
    int result = a + b;
    float pi = 3.14;
    int arr = [1, 2, 3];
    string message = "The result is: ";
    Console.WriteLine(message);
    return result;
}

func void printMessage(string msg, int count) {
    Console.WriteLine(msg);
}

// Function calls with different parameter types
int sum = calculateSum(10, 20);
printMessage("Hello World", 5);
""";
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
