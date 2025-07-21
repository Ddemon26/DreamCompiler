package com.dream.parser;

import com.intellij.lang.ASTNode;
import com.intellij.lang.PsiBuilder;
import com.intellij.lang.PsiParser;
import com.intellij.psi.tree.IElementType;
import org.jetbrains.annotations.NotNull;
import com.dream.psi.DreamTypes;

public class DreamParser implements PsiParser {
    
    @NotNull
    @Override
    public ASTNode parse(IElementType root, PsiBuilder builder) {
        PsiBuilder.Marker rootMarker = builder.mark();
        
        // Parse the compilation unit - just consume all tokens for now
        while (!builder.eof()) {
            // Simple parsing - just advance through tokens
            // In a real implementation, this would have proper grammar rules
            builder.advanceLexer();
        }
        
        rootMarker.done(root);
        return builder.getTreeBuilt();
    }
}