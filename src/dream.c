#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    static char *strndup(const char *src, size_t n)
    {
        size_t len = strlen(src);
        if (n < len) len = n;

        char *dest = malloc(len + 1);
        if (!dest) return NULL;

        memcpy(dest, src, len);
        dest[len] = '\0';
        return dest;
    }
#endif

// Token types
typedef enum {
    TOKEN_INT, TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_PLUS, TOKEN_SEMICOLON,
    TOKEN_EQUALS, TOKEN_CONSOLE, TOKEN_DOT, TOKEN_WRITELINE, TOKEN_LPAREN,
    TOKEN_RPAREN, TOKEN_EOF, TOKEN_UNKNOWN
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value;
} Token;

// Node types for AST
typedef enum {
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_WRITELINE,
    NODE_BINARY_OP,
    NODE_NUMBER
} NodeType;

// AST node structure
typedef struct Node {
    NodeType type;
    char* value; // For identifiers, numbers, or operators
    struct Node* left;
    struct Node* right;
} Node;

// Lexer state
typedef struct {
    char* source;
    int pos;
    int line;
} Lexer;

// Compiler context (only output file needed for C generation)
typedef struct {
    FILE* output;
} Compiler;

// Lexer functions
Token next_token(Lexer* lexer) {
    while (isspace(lexer->source[lexer->pos])) lexer->pos++;

    // Skip // comments
    if (lexer->source[lexer->pos] == '/' && lexer->source[lexer->pos + 1] == '/') {
        while (lexer->source[lexer->pos] && lexer->source[lexer->pos] != '\n') lexer->pos++;
        return next_token(lexer);
    }

    Token token = {TOKEN_UNKNOWN, NULL};
    if (lexer->source[lexer->pos] == '\0') {
        token.type = TOKEN_EOF;
        return token;
    }

    if (isalpha(lexer->source[lexer->pos])) {
        int start = lexer->pos;
        while (isalnum(lexer->source[lexer->pos])) lexer->pos++;
        int len = lexer->pos - start;
        token.value = strndup(lexer->source + start, len);
        if (strcmp(token.value, "int") == 0) token.type = TOKEN_INT;
        else if (strcmp(token.value, "Console") == 0) token.type = TOKEN_CONSOLE;
        else if (strcmp(token.value, "WriteLine") == 0) token.type = TOKEN_WRITELINE;
        else token.type = TOKEN_IDENTIFIER;
        return token;
    }

    if (isdigit(lexer->source[lexer->pos])) {
        int start = lexer->pos;
        while (isdigit(lexer->source[lexer->pos])) lexer->pos++;
        int len = lexer->pos - start;
        token.value = strndup(lexer->source + start, len);
        token.type = TOKEN_NUMBER;
        return token;
    }

    switch (lexer->source[lexer->pos]) {
        case '+': token.type = TOKEN_PLUS; token.value = strdup("+"); lexer->pos++; break;
        case ';': token.type = TOKEN_SEMICOLON; token.value = strdup(";"); lexer->pos++; break;
        case '=': token.type = TOKEN_EQUALS; token.value = strdup("="); lexer->pos++; break;
        case '.': token.type = TOKEN_DOT; token.value = strdup("."); lexer->pos++; break;
        case '(': token.type = TOKEN_LPAREN; token.value = strdup("("); lexer->pos++; break;
        case ')': token.type = TOKEN_RPAREN; token.value = strdup(")"); lexer->pos++; break;
        default: token.type = TOKEN_UNKNOWN; token.value = strndup(lexer->source + lexer->pos++, 1); break;
    }
    return token;
}

// Parser functions
Node* create_node(NodeType type, char* value, Node* left, Node* right) {
    Node* node = malloc(sizeof(Node));
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->left = left;
    node->right = right;
    return node;
}

// Parse a simple expression: identifier/number with optional + identifier/number
Node* parse_expression(Lexer* lexer, Token* token) {
    if (token->type != TOKEN_IDENTIFIER && token->type != TOKEN_NUMBER) {
        fprintf(stderr, "Expected identifier or number\n");
        exit(1);
    }
    Node* left = create_node(token->type == TOKEN_IDENTIFIER ? NODE_VAR_DECL : NODE_NUMBER, token->value, NULL, NULL);
    *token = next_token(lexer);
    if (token->type == TOKEN_PLUS) {
        char* op = token->value;
        *token = next_token(lexer);
        if (token->type != TOKEN_IDENTIFIER && token->type != TOKEN_NUMBER) {
            fprintf(stderr, "Expected identifier or number after +\n");
            exit(1);
        }
        Node* right = create_node(token->type == TOKEN_IDENTIFIER ? NODE_VAR_DECL : NODE_NUMBER, token->value, NULL, NULL);
        *token = next_token(lexer);
        return create_node(NODE_BINARY_OP, op, left, right);
    }
    return left;
}

Node* parse_statement(Lexer* lexer, Token* token) {
    if (token->type == TOKEN_INT) {
        free(token->value);
        *token = next_token(lexer);
        if (token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Expected identifier after int\n");
            exit(1);
        }
        char* var_name = token->value;
        *token = next_token(lexer);
        Node* init = NULL;
        if (token->type == TOKEN_EQUALS) {
            free(token->value);
            *token = next_token(lexer);
            init = parse_expression(lexer, token);
        }
        if (token->type != TOKEN_SEMICOLON) {
            fprintf(stderr, "Expected semicolon\n");
            exit(1);
        }
        return create_node(NODE_VAR_DECL, var_name, init, NULL);
    } else if (token->type == TOKEN_IDENTIFIER) {
        char* var_name = token->value;
        *token = next_token(lexer);
        if (token->type == TOKEN_EQUALS) {
            free(token->value);
            *token = next_token(lexer);
            Node* expr = parse_expression(lexer, token);
            if (token->type != TOKEN_SEMICOLON) {
                fprintf(stderr, "Expected semicolon\n");
                exit(1);
            }
            return create_node(NODE_ASSIGN, var_name, expr, NULL);
        }
    } else if (token->type == TOKEN_CONSOLE) {
        free(token->value);
        *token = next_token(lexer);
        if (token->type != TOKEN_DOT) {
            fprintf(stderr, "Expected dot after Console\n");
            exit(1);
        }
        free(token->value);
        *token = next_token(lexer);
        if (token->type != TOKEN_WRITELINE) {
            fprintf(stderr, "Expected WriteLine\n");
            exit(1);
        }
        free(token->value);
        *token = next_token(lexer);
        if (token->type != TOKEN_LPAREN) {
            fprintf(stderr, "Expected (\n");
            exit(1);
        }
        free(token->value);
        *token = next_token(lexer);
        if (token->type != TOKEN_IDENTIFIER && token->type != TOKEN_NUMBER) {
            fprintf(stderr, "Expected identifier or number\n");
            exit(1);
        }
        Node* arg = create_node(token->type == TOKEN_IDENTIFIER ? NODE_VAR_DECL : NODE_BINARY_OP, token->value, NULL, NULL);
        *token = next_token(lexer);
        if (token->type != TOKEN_RPAREN) {
            fprintf(stderr, "Expected )\n");
            exit(1);
        }
        free(token->value);
        *token = next_token(lexer);
        if (token->type != TOKEN_SEMICOLON) {
            fprintf(stderr, "Expected semicolon\n");
            exit(1);
        }
        return create_node(NODE_WRITELINE, NULL, arg, NULL);
    }
    fprintf(stderr, "Invalid statement\n");
    exit(1);
}

// Generate a C expression recursively
static void gen_c_expr(FILE* out, Node* expr) {
    if (!expr) return;
    if (expr->type == NODE_BINARY_OP) {
        fputc('(', out);
        gen_c_expr(out, expr->left);
        fprintf(out, " %s ", expr->value);
        gen_c_expr(out, expr->right);
        fputc(')', out);
    } else if (expr->type == NODE_VAR_DECL && expr->left == NULL && expr->right == NULL) {
        fprintf(out, "%s", expr->value);
    } else if (expr->type == NODE_NUMBER) {
        fprintf(out, "%s", expr->value);
    }
}

// Generate C statements
static void generate_c(Compiler* compiler, Node* node) {
    FILE* out = compiler->output;
    if (node->type == NODE_VAR_DECL) {
        fprintf(out, "    long %s", node->value);
        if (node->left) {
            fprintf(out, " = ");
            gen_c_expr(out, node->left);
        }
        fprintf(out, ";\n");
    } else if (node->type == NODE_ASSIGN) {
        fprintf(out, "    %s = ", node->value);
        gen_c_expr(out, node->left);
        fprintf(out, ";\n");
    } else if (node->type == NODE_WRITELINE) {
        fprintf(out, "    printf(\"%%ld\\n\", ");
        gen_c_expr(out, node->left);
        fprintf(out, ");\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input.dr>\n", argv[0]);
        return 1;
    }

    // Read source file
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        return 1;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* source = malloc(size + 1);
    fread(source, 1, size, file);
    source[size] = '\0';
    fclose(file);

    // Initialize lexer and compiler
    Lexer lexer = {source, 0, 1};
    if ((unsigned char)source[0] == 0xEF && (unsigned char)source[1] == 0xBB && (unsigned char)source[2] == 0xBF) {
        lexer.pos = 3; // Skip UTF-8 BOM
    }
    Compiler compiler = {fopen("output.c", "w")};

    // Write C preamble
    fprintf(compiler.output, "#include <stdio.h>\n");
    fprintf(compiler.output, "int main() {\n");

    // Parse and generate code
    Token token = next_token(&lexer);
    while (token.type != TOKEN_EOF) {
        Node* node = parse_statement(&lexer, &token);
        generate_c(&compiler, node);
        free(node->value);
        if (node->left) free(node->left->value), free(node->left);
        if (node->right) free(node->right->value), free(node->right);
        free(node);
        token = next_token(&lexer);
    }

    // Write C epilogue
    fprintf(compiler.output, "    return 0;\n");
    fprintf(compiler.output, "}\n");

    fclose(compiler.output);
    free(source);
    free(token.value);

    // Compile generated C code
    system("gcc output.c -o dream");
    return 0;
}