#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

// Compiler context
typedef struct {
    FILE* output;
    int var_count;
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

// Evaluate an expression and leave the result in RAX
static void gen_expr(Compiler* compiler, Node* expr, char** var_map) {
    if (!expr) return;
    if (expr->type == NODE_BINARY_OP) {
        gen_expr(compiler, expr->left, var_map);
        fprintf(compiler->output, "    push rax\n");
        gen_expr(compiler, expr->right, var_map);
        fprintf(compiler->output, "    mov rbx, rax\n");
        fprintf(compiler->output, "    pop rax\n");
        if (strcmp(expr->value, "+") == 0) {
            fprintf(compiler->output, "    add rax, rbx\n");
        }
    } else if (expr->type == NODE_VAR_DECL) {
        for (int i = 0; i < compiler->var_count; i++) {
            if (strcmp(var_map[i], expr->value) == 0) {
                fprintf(compiler->output, "    mov rax, [rbp-%d]\n", (i + 1) * 8);
                break;
            }
        }
    } else if (expr->type == NODE_NUMBER) {
        fprintf(compiler->output, "    mov rax, %s\n", expr->value);
    } else {
        fprintf(stderr, "Unknown expression type\n");
    }
}

// Code generation
void generate_assembly(Compiler* compiler, Node* node) {
    static int var_index = 0;
    static char* var_map[100];

    if (node->type == NODE_VAR_DECL) {
        var_map[var_index++] = strdup(node->value);
        if (node->left) {
            gen_expr(compiler, node->left, var_map);
        } else {
            fprintf(compiler->output, "    mov rax, 0\n");
        }
        fprintf(compiler->output, "    mov [rbp-%d], rax\n", var_index * 8);
        compiler->var_count++;
    } else if (node->type == NODE_ASSIGN) {
        gen_expr(compiler, node->left, var_map);
        for (int i = 0; i < compiler->var_count; i++) {
            if (strcmp(var_map[i], node->value) == 0) {
                fprintf(compiler->output, "    mov [rbp-%d], rax\n", (i + 1) * 8);
                break;
            }
        }
    } else if (node->type == NODE_WRITELINE) {
        gen_expr(compiler, node->left, var_map);
        fprintf(compiler->output, "    mov rsi, rax\n");
        fprintf(compiler->output, "    mov rdi, fmt\n");
        fprintf(compiler->output, "    xor rax, rax\n");
        fprintf(compiler->output, "    call printf\n");
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
    Compiler compiler = {fopen("output.asm", "w"), 0};

    // Write assembly preamble
    fprintf(compiler.output, "extern printf\n");
    fprintf(compiler.output, "section .data\n");
    fprintf(compiler.output, "fmt db \"%%ld\",10,0\n");
    fprintf(compiler.output, "section .text\n");
    fprintf(compiler.output, "global main\n");
    fprintf(compiler.output, "main:\n");
    fprintf(compiler.output, "    sub rsp, 1032\n"); // Reserve stack space and align
    fprintf(compiler.output, "    mov rbp, rsp\n");

    // Parse and generate code
    Token token = next_token(&lexer);
    while (token.type != TOKEN_EOF) {
        Node* node = parse_statement(&lexer, &token);
        generate_assembly(&compiler, node);
        free(node->value);
        if (node->left) free(node->left->value), free(node->left);
        if (node->right) free(node->right->value), free(node->right);
        free(node);
        token = next_token(&lexer);
    }

    // Write assembly epilogue
    fprintf(compiler.output, "    add rsp, 1032\n");
    fprintf(compiler.output, "    mov rax, 0\n");
    fprintf(compiler.output, "    ret\n");

    fclose(compiler.output);
    free(source);
    free(token.value);

    // Assemble and link
    system("nasm -f elf64 output.asm -o output.o");
    system("gcc output.o -no-pie -o dream");
    return 0;
}