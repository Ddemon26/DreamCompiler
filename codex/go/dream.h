#ifndef DREAM_H
#define DREAM_H

#include <stddef.h>
#include <stdbool.h>

// Minimal C API interface for Go bindings
// All types will be handled as opaque pointers or through specific API functions

// API Result codes
typedef enum {
    DREAM_OK = 0,
    DREAM_ERROR_LEX = 1,
    DREAM_ERROR_PARSE = 2,
    DREAM_ERROR_SEM = 3,
    DREAM_ERROR_CODEGEN = 4,
    DREAM_ERROR_IO = 5
} DreamResult;

// Opaque type declarations for Go bindings
typedef struct DreamLexer DreamLexer;
typedef struct DreamParser DreamParser;
typedef struct DreamNode DreamNode;

// C API functions for Go bindings
DreamLexer* dream_lexer_new(const char* input);
void dream_lexer_free(DreamLexer* lexer);
DreamResult dream_lexer_next_token(DreamLexer* lexer, int* token_kind, const char** token_text, size_t* token_len);

DreamParser* dream_parser_new(DreamLexer* lexer);
void dream_parser_free(DreamParser* parser);
DreamResult dream_parser_parse(DreamParser* parser, DreamNode** root);

void dream_node_free(DreamNode* node);
int dream_node_get_kind(DreamNode* node);
const char* dream_node_to_string(DreamNode* node);

DreamResult dream_compile_to_c(const char* input, const char* output_file);

#endif // DREAM_H