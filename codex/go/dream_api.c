/*
 * DreamCompiler C API Implementation for Go Testing
 * This file implements the C API wrapper functions for Go cgo integration
 */

#include "dream.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Include compiler headers after defining our API
#include "../../src/lexer/lexer.h"
#include "../../src/parser/ast.h"
#include "../../src/parser/parser.h"
#include "../../src/sem/type.h"

// Include Arena functions
extern void arena_init(Arena* arena);

// Wrapper structures that map to compiler types
struct DreamLexer {
    Lexer lexer;
};

struct DreamParser {
    Parser parser;
    Arena arena;
    DreamLexer* dream_lexer;
    const char* source;
};

struct DreamNode {
    Node* node;
};

// Lexer API implementation
DreamLexer* dream_lexer_new(const char* input) {
    if (!input) return NULL;
    
    DreamLexer* dream_lexer = malloc(sizeof(DreamLexer));
    if (!dream_lexer) return NULL;
    
    lexer_init(&dream_lexer->lexer, input);
    return dream_lexer;
}

void dream_lexer_free(DreamLexer* lexer) {
    if (lexer) {
        free(lexer);
    }
}

DreamResult dream_lexer_next_token(DreamLexer* lexer, int* token_kind, const char** token_text, size_t* token_len) {
    if (!lexer || !token_kind || !token_text || !token_len) {
        return DREAM_ERROR_LEX;
    }
    
    Token token = lexer_next(&lexer->lexer);
    *token_kind = token.kind;
    *token_text = token.start;
    *token_len = token.len;
    
    return DREAM_OK;
}

// Parser API implementation
DreamParser* dream_parser_new(DreamLexer* lexer) {
    if (!lexer) return NULL;
    
    DreamParser* dream_parser = malloc(sizeof(DreamParser));
    if (!dream_parser) return NULL;
    
    dream_parser->dream_lexer = lexer;
    dream_parser->source = lexer->lexer.src;
    
    arena_init(&dream_parser->arena);
    parser_init(&dream_parser->parser, &dream_parser->arena, dream_parser->source);
    
    return dream_parser;
}

void dream_parser_free(DreamParser* parser) {
    if (parser) {
        // Note: Arena memory may be cleaned up automatically or managed by compiler
        // For now, just free the wrapper structure
        free(parser);
    }
}

DreamResult dream_parser_parse(DreamParser* parser, DreamNode** root) {
    if (!parser || !root) {
        return DREAM_ERROR_PARSE;
    }
    
    Node* node = parse_program(&parser->parser);
    if (!node) {
        return DREAM_ERROR_PARSE;
    }
    
    DreamNode* dream_node = malloc(sizeof(DreamNode));
    if (!dream_node) {
        return DREAM_ERROR_PARSE;
    }
    
    dream_node->node = node;
    *root = dream_node;
    
    return DREAM_OK;
}

// Node API implementation
void dream_node_free(DreamNode* node) {
    if (node) {
        // Note: The underlying Node* may be managed by an arena,
        // so we only free our wrapper
        free(node);
    }
}

int dream_node_get_kind(DreamNode* node) {
    if (!node || !node->node) {
        return -1;
    }
    return node->node->kind;
}

const char* dream_node_to_string(DreamNode* node) {
    if (!node || !node->node) {
        return "null";
    }
    
    // Simple string representation based on node kind
    switch (node->node->kind) {
        case ND_INT: return "int_literal";
        case ND_FLOAT: return "float_literal";
        case ND_STRING: return "string_literal";
        case ND_CHAR: return "char_literal";
        case ND_BOOL: return "bool_literal";
        case ND_IDENT: return "identifier";
        case ND_BINOP: return "binary_op";
        case ND_UNARY: return "unary_op";
        case ND_CALL: return "function_call";
        case ND_IF: return "if_statement";
        case ND_WHILE: return "while_loop";
        case ND_FOR: return "for_loop";
        case ND_RETURN: return "return_statement";
        case ND_BLOCK: return "block";
        case ND_VAR_DECL: return "variable_declaration";
        case ND_FUNC: return "function_declaration";
        case ND_MODULE: return "module";
        default: return "unknown_node";
    }
}

// High-level compilation API
DreamResult dream_compile_to_c(const char* input, const char* output_file) {
    if (!input || !output_file) {
        return DREAM_ERROR_IO;
    }
    
    // Create lexer
    DreamLexer* lexer = dream_lexer_new(input);
    if (!lexer) {
        return DREAM_ERROR_LEX;
    }
    
    // Create parser
    DreamParser* parser = dream_parser_new(lexer);
    if (!parser) {
        dream_lexer_free(lexer);
        return DREAM_ERROR_PARSE;
    }
    
    // Parse input
    DreamNode* root = NULL;
    DreamResult result = dream_parser_parse(parser, &root);
    if (result != DREAM_OK) {
        dream_parser_free(parser);
        dream_lexer_free(lexer);
        return result;
    }
    
    // TODO: Add semantic analysis and code generation
    // For now, just return success if parsing worked
    
    // Cleanup
    dream_node_free(root);
    dream_parser_free(parser);
    dream_lexer_free(lexer);
    
    return DREAM_OK;
}