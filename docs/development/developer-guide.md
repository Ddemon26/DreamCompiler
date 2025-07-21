# Dream Compiler Developer Guide

This comprehensive guide is designed for developers who want to understand, extend, or contribute to the Dream Compiler project. It covers the complete architecture, development workflows, and implementation details.

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Development Environment Setup](#development-environment-setup)
3. [Compiler Pipeline](#compiler-pipeline)
4. [Source Code Organization](#source-code-organization)
5. [Development Workflow](#development-workflow)
6. [Adding Language Features](#adding-language-features)
7. [Testing and Validation](#testing-and-validation)
8. [Performance Optimization](#performance-optimization)
9. [Contributing Guidelines](#contributing-guidelines)
10. [Advanced Topics](#advanced-topics)

---

## Architecture Overview

The Dream Compiler follows a traditional multi-phase compilation architecture with modern optimizations and cross-platform support.

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    Dream Compiler Architecture                  │
├─────────────────────────────────────────────────────────────────┤
│  Source Files (.dr)                                            │
│    ↓                                                            │
│  Lexical Analysis (re2c-generated lexer)                       │
│    ↓                                                            │
│  Syntax Analysis (Pratt parser with error recovery)            │
│    ↓                                                            │
│  Semantic Analysis (Type checking, symbol resolution)          │
│    ↓                                                            │
│  IR Generation (SSA-based three-address code)                  │
│    ↓                                                            │
│  Optimization Pipeline (SCCP, DCE, LICM, CSE, etc.)          │
│    ↓                                                            │
│  Code Generation (Clean C code output)                         │
│    ↓                                                            │
│  C Compilation (via zig cc)                                    │
│    ↓                                                            │
│  Native Executable                                             │
└─────────────────────────────────────────────────────────────────┘
```

### Core Design Principles

1. **Separation of Concerns**: Each compilation phase is isolated with clear interfaces
2. **Extensibility**: Modular design allows easy addition of new features
3. **Performance**: Fast compilation with incremental and parallel processing
4. **Correctness**: Comprehensive error checking and recovery mechanisms
5. **Maintainability**: Well-structured code with extensive documentation

### Technology Stack

- **Build System**: Zig (cross-platform, fast builds)
- **Implementation Language**: C11 (maximum compatibility)
- **Lexer Generation**: re2c (high-performance lexical analysis)
- **Parser**: Hand-written Pratt parser (excellent error recovery)
- **Target Platform**: Portable C code (runs anywhere)

---

## Development Environment Setup

### Prerequisites

```bash
# Required tools
- Zig 0.12.0 or later
- GCC or Clang compiler
- re2c (lexer generator)
- Git
- Python 3.8+ (for testing scripts)

# Optional tools (recommended)
- Valgrind (Linux memory debugging)
- Perf (Linux profiling)
- Visual Studio Code with C/C++ extension
- LLDB or GDB debugger
```

### Initial Setup

```bash
# Clone the repository
git clone https://github.com/company/DreamCompiler.git
cd DreamCompiler

# Build the compiler
zig build

# Run quick tests to verify setup
./codex/test_cli.sh quick  # Linux/macOS
# OR
.\codex\test_cli.ps1 quick  # Windows

# Generate lexer (if modified lexer.re)
re2c -c -o src/lexer/lexer.c src/lexer/lexer.re

# Build documentation (optional)
zig build docs
```

### IDE Configuration

#### Visual Studio Code (`/.vscode/settings.json`)

```json
{
    "C_Cpp.default.compilerPath": "/usr/bin/gcc",
    "C_Cpp.default.cStandard": "c11",
    "C_Cpp.default.includePath": [
        "${workspaceFolder}/src",
        "${workspaceFolder}/src/lexer",
        "${workspaceFolder}/src/parser",
        "${workspaceFolder}/src/sem",
        "${workspaceFolder}/src/ir",
        "${workspaceFolder}/src/codegen"
    ],
    "C_Cpp.default.defines": [
        "DEBUG=1",
        "DREAM_COMPILER"
    ],
    "files.associations": {
        "*.re": "c",
        "*.def": "c",
        "*.dr": "csharp"
    },
    "editor.tabSize": 4,
    "editor.insertSpaces": true
}
```

#### VS Code Tasks (`/.vscode/tasks.json`)

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build Compiler",
            "type": "shell",
            "command": "zig",
            "args": ["build"],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": []
        },
        {
            "label": "Run Quick Tests",
            "type": "shell",
            "command": "./codex/test_cli.sh",
            "args": ["quick"],
            "group": "test",
            "dependsOn": "Build Compiler"
        },
        {
            "label": "Generate Lexer",
            "type": "shell",
            "command": "re2c",
            "args": ["-c", "-o", "src/lexer/lexer.c", "src/lexer/lexer.re"],
            "group": "build"
        }
    ]
}
```

### Debug Configuration (`/.vscode/launch.json`)

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Compiler",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/zig-out/bin/DreamCompiler",
            "args": ["tests/basics/io/console.dr"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        },
        {
            "name": "Debug Generated Code",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/bin/dream",
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}/build/bin",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb"
        }
    ]
}
```

---

## Compiler Pipeline

Understanding the compilation pipeline is crucial for developers working on the Dream Compiler.

### Phase 1: Lexical Analysis

**Location**: `src/lexer/`

The lexer converts source text into tokens using a re2c-generated finite automaton.

```c
// Token structure
typedef struct {
    TokenType type;
    char* text;
    int length;
    int line;
    int column;
    SourceLocation location;
} Token;

// Main lexer function
Token* lexer_next_token(Lexer* lexer);
```

**Key Files:**
- `lexer.re` - re2c specification (source of truth)
- `lexer.c` - Generated lexer implementation
- `token.c` - Token management and utilities
- `tokens.def` - Token type definitions

**Example Token Processing:**
```c
void process_tokens(const char* source) {
    Lexer* lexer = lexer_create(source);
    Token* token;
    
    while ((token = lexer_next_token(lexer))->type != TOKEN_EOF) {
        printf("Token: %s (%s) at %d:%d\n", 
               token_type_name(token->type),
               token->text,
               token->line, 
               token->column);
        
        token_destroy(token);
    }
    
    lexer_destroy(lexer);
}
```

### Phase 2: Syntax Analysis

**Location**: `src/parser/`

The parser builds an Abstract Syntax Tree (AST) using a Pratt parser for excellent error recovery.

```c
// AST node structure
typedef struct ASTNode {
    NodeType type;
    SourceLocation location;
    union {
        BinaryExpr binary;
        UnaryExpr unary;
        FunctionDecl function;
        VarDecl variable;
        // ... other node types
    } data;
    struct ASTNode** children;
    int child_count;
} ASTNode;

// Main parsing functions
ASTNode* parse_program(Parser* parser);
ASTNode* parse_expression(Parser* parser, int min_precedence);
ASTNode* parse_statement(Parser* parser);
```

**Pratt Parser Implementation:**
```c
// Precedence-based expression parsing
ASTNode* parse_expression(Parser* parser, int min_precedence) {
    ASTNode* left = parse_primary(parser);
    
    while (current_token_precedence(parser) >= min_precedence) {
        Token* op = parser_current_token(parser);
        int precedence = token_precedence(op->type);
        
        parser_advance(parser);
        ASTNode* right = parse_expression(parser, precedence + 1);
        
        left = create_binary_expr(op->type, left, right);
    }
    
    return left;
}
```

**Error Recovery Strategy:**
```c
// Synchronization points for error recovery
void parser_synchronize(Parser* parser) {
    parser_advance(parser); // Skip erroneous token
    
    while (!parser_at_end(parser)) {
        if (parser_previous_token(parser)->type == TOKEN_SEMICOLON) {
            return; // Found statement boundary
        }
        
        // Synchronize on statement keywords
        switch (parser_current_token(parser)->type) {
            case TOKEN_IF:
            case TOKEN_FOR:
            case TOKEN_WHILE:
            case TOKEN_FUNC:
            case TOKEN_CLASS:
                return;
            default:
                parser_advance(parser);
        }
    }
}
```

### Phase 3: Semantic Analysis

**Location**: `src/sem/`

Semantic analysis performs type checking, symbol resolution, and semantic validation.

```c
// Symbol table structure
typedef struct SymbolTable {
    HashMap* symbols;           // Current scope symbols
    struct SymbolTable* parent; // Parent scope
    ScopeType type;            // Function, class, block, etc.
} SymbolTable;

// Type information
typedef struct TypeInfo {
    TypeKind kind;             // INT, FLOAT, STRING, CLASS, etc.
    char* name;                // Type name
    struct TypeInfo* element_type; // For arrays
    struct ClassInfo* class_info;  // For classes
} TypeInfo;

// Main semantic analysis functions
bool semantic_analyze(ASTNode* root, SymbolTable* global_scope);
TypeInfo* type_check_expression(ASTNode* expr, SymbolTable* scope);
bool resolve_symbols(ASTNode* node, SymbolTable* scope);
```

**Type Checking Implementation:**
```c
TypeInfo* type_check_binary_expr(BinaryExpr* expr, SymbolTable* scope) {
    TypeInfo* left_type = type_check_expression(expr->left, scope);
    TypeInfo* right_type = type_check_expression(expr->right, scope);
    
    if (!left_type || !right_type) {
        return NULL; // Error in operands
    }
    
    switch (expr->operator) {
        case OP_PLUS:
        case OP_MINUS:
        case OP_MULTIPLY:
        case OP_DIVIDE:
            return check_arithmetic_types(left_type, right_type, expr->location);
            
        case OP_EQUALS:
        case OP_NOT_EQUALS:
            return check_equality_types(left_type, right_type, expr->location);
            
        case OP_AND:
        case OP_OR:
            return check_logical_types(left_type, right_type, expr->location);
            
        default:
            semantic_error("Unknown binary operator", expr->location);
            return NULL;
    }
}
```

### Phase 4: IR Generation

**Location**: `src/ir/`, `src/ssa/`, `src/cfg/`

Converts the AST into Static Single Assignment (SSA) intermediate representation.

```c
// IR instruction structure
typedef struct IRInstruction {
    IROpcode opcode;
    IRValue* dest;
    IRValue* operand1;
    IRValue* operand2;
    struct IRInstruction* next;
} IRInstruction;

// Basic block structure
typedef struct BasicBlock {
    int id;
    IRInstruction* instructions;
    struct BasicBlock** successors;
    struct BasicBlock** predecessors;
    int successor_count;
    int predecessor_count;
} BasicBlock;

// Control Flow Graph
typedef struct CFG {
    BasicBlock** blocks;
    int block_count;
    BasicBlock* entry;
    BasicBlock* exit;
} CFG;
```

**IR Generation Example:**
```c
IRValue* generate_binary_expr(BinaryExpr* expr, IRBuilder* builder) {
    IRValue* left = generate_expression(expr->left, builder);
    IRValue* right = generate_expression(expr->right, builder);
    IRValue* result = ir_create_temporary(expr->result_type);
    
    IRInstruction* instr = ir_create_instruction(
        get_ir_opcode(expr->operator),
        result, left, right
    );
    
    ir_builder_emit(builder, instr);
    return result;
}
```

### Phase 5: Optimization

**Location**: `src/opt/`

Multiple optimization passes improve the generated code quality.

**Available Optimizations:**
1. **SCCP** (Sparse Conditional Constant Propagation)
2. **DCE** (Dead Code Elimination)
3. **LICM** (Loop Invariant Code Motion)
4. **CSE** (Common Subexpression Elimination)
5. **Constant Folding**
6. **Copy Propagation**

```c
// Optimization pass interface
typedef struct OptimizationPass {
    const char* name;
    bool (*run)(CFG* cfg);
    bool (*should_run)(CFG* cfg);
} OptimizationPass;

// Constant propagation implementation
bool constant_propagation_pass(CFG* cfg) {
    bool changed = false;
    
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock* block = cfg->blocks[i];
        
        for (IRInstruction* instr = block->instructions; instr; instr = instr->next) {
            if (is_arithmetic_op(instr->opcode)) {
                if (instr->operand1->type == IR_CONSTANT && 
                    instr->operand2->type == IR_CONSTANT) {
                    
                    // Fold constant arithmetic
                    IRValue* folded = fold_constants(instr);
                    replace_instruction(instr, folded);
                    changed = true;
                }
            }
        }
    }
    
    return changed;
}
```

### Phase 6: Code Generation

**Location**: `src/codegen/`

Generates clean, readable C code from the optimized IR.

```c
// Code generation context
typedef struct CodeGenerator {
    FILE* output;
    SymbolTable* symbols;
    int indent_level;
    bool in_function;
    char* current_function_name;
} CodeGenerator;

// Main code generation functions
void generate_c_code(CFG* cfg, const char* output_path);
void generate_function(FunctionDecl* func, CodeGenerator* gen);
void generate_statement(ASTNode* stmt, CodeGenerator* gen);
void generate_expression(ASTNode* expr, CodeGenerator* gen);
```

**C Code Generation Example:**
```c
void generate_binary_expr(BinaryExpr* expr, CodeGenerator* gen) {
    fprintf(gen->output, "(");
    generate_expression(expr->left, gen);
    
    switch (expr->operator) {
        case OP_PLUS:     fprintf(gen->output, " + "); break;
        case OP_MINUS:    fprintf(gen->output, " - "); break;
        case OP_MULTIPLY: fprintf(gen->output, " * "); break;
        case OP_DIVIDE:   fprintf(gen->output, " / "); break;
        case OP_EQUALS:   fprintf(gen->output, " == "); break;
        case OP_AND:      fprintf(gen->output, " && "); break;
        // ... other operators
    }
    
    generate_expression(expr->right, gen);
    fprintf(gen->output, ")");
}
```

---

## Source Code Organization

The Dream Compiler codebase is organized for maximum clarity and maintainability.

### Directory Structure

```
src/
├── main.c                    # Entry point
├── driver/                   # Command-line interface
│   ├── driver.c             # Main driver logic
│   └── cli.c                # Command-line parsing
├── lexer/                   # Lexical analysis
│   ├── lexer.re             # re2c specification
│   ├── lexer.c              # Generated lexer
│   ├── token.c              # Token utilities
│   └── tokens.def           # Token definitions
├── parser/                  # Syntax analysis
│   ├── parser.c             # Main parser implementation
│   ├── ast.c                # AST node management
│   └── precedence.c         # Operator precedence
├── sem/                     # Semantic analysis
│   ├── semantic.c           # Main semantic analyzer
│   ├── symbol_table.c       # Symbol table management
│   └── type_checker.c       # Type checking logic
├── ir/                      # Intermediate representation
│   ├── ir.c                 # IR instruction management
│   └── ir_builder.c         # IR construction utilities
├── ssa/                     # Static Single Assignment
│   └── ssa.c                # SSA form conversion
├── cfg/                     # Control Flow Graph
│   └── cfg.c                # CFG construction and analysis
├── opt/                     # Optimization passes
│   ├── optimizer.c          # Optimization pipeline
│   ├── constant_folding.c   # Constant folding pass
│   ├── dead_code_elimination.c  # DCE pass
│   ├── sccp.c               # SCCP pass
│   ├── licm.c               # Loop invariant code motion
│   └── cse.c                # Common subexpression elimination
├── codegen/                 # Code generation
│   ├── codegen.c            # Main code generator
│   ├── c_emitter.c          # C code emission
│   ├── module.c             # Multi-file module support
│   └── backend.c            # Backend abstraction
├── utils/                   # Utility functions
│   ├── memory.c             # Memory management
│   ├── string_utils.c       # String utilities
│   ├── file_utils.c         # File operations
│   ├── error.c              # Error reporting
│   └── debug.c              # Debug utilities
└── runtime/                 # Runtime library
    ├── memory.c             # Runtime memory management
    ├── console.c            # Console I/O
    └── custom.c             # Custom runtime functions
```

### Header Organization

Headers are organized to minimize dependencies and compilation time:

```c
// Core headers
#include "common.h"          // Common definitions
#include "token.h"           // Token types and functions
#include "ast.h"             // AST node definitions
#include "symbol_table.h"    // Symbol table interface
#include "ir.h"              # IR definitions
#include "codegen.h"         # Code generation interface
```

### Coding Standards

#### Naming Conventions

```c
// Functions: snake_case
void parse_expression(Parser* parser);
IRValue* generate_binary_expr(BinaryExpr* expr);

// Types: PascalCase
typedef struct ASTNode ASTNode;
typedef struct TypeInfo TypeInfo;

// Constants: UPPER_SNAKE_CASE
#define MAX_TOKEN_LENGTH 256
#define DEFAULT_SYMBOL_TABLE_SIZE 64

// Variables: snake_case
int current_line = 1;
char* function_name = NULL;

// Macros: UPPER_SNAKE_CASE
#define ASSERT(condition) \
    do { if (!(condition)) abort(); } while(0)
```

#### Memory Management Pattern

```c
// Constructor pattern
Parser* parser_create(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->lexer = lexer;
    parser->current_token = NULL;
    parser->had_error = false;
    
    return parser;
}

// Destructor pattern
void parser_destroy(Parser* parser) {
    if (!parser) return;
    
    if (parser->current_token) {
        token_destroy(parser->current_token);
    }
    
    free(parser);
}

// RAII-style usage
void compile_file(const char* filename) {
    Lexer* lexer = lexer_create_from_file(filename);
    if (!lexer) return;
    
    Parser* parser = parser_create(lexer);
    if (!parser) {
        lexer_destroy(lexer);
        return;
    }
    
    ASTNode* root = parse_program(parser);
    // ... use root ...
    
    ast_destroy(root);
    parser_destroy(parser);
    lexer_destroy(lexer);
}
```

#### Error Handling Pattern

```c
// Result type for functions that can fail
typedef enum {
    RESULT_SUCCESS,
    RESULT_ERROR_MEMORY,
    RESULT_ERROR_SYNTAX,
    RESULT_ERROR_SEMANTIC,
    RESULT_ERROR_IO
} ResultCode;

typedef struct {
    ResultCode code;
    char* error_message;
    SourceLocation location;
} Result;

// Function that can fail
Result parse_declaration(Parser* parser, ASTNode** out_node) {
    if (!expect_token(parser, TOKEN_FUNC)) {
        return error_result(RESULT_ERROR_SYNTAX, 
                          "Expected 'func' keyword",
                          parser->current_token->location);
    }
    
    // ... parsing logic ...
    
    *out_node = node;
    return success_result();
}

// Usage pattern
Result result = parse_declaration(parser, &node);
if (result.code != RESULT_SUCCESS) {
    report_error(result.error_message, result.location);
    return result;
}
```

---

## Development Workflow

### Branch Strategy

The Dream Compiler uses a feature branch workflow:

```bash
# Main branches
main        # Stable releases
develop     # Integration branch for new features
hotfix/*    # Critical bug fixes

# Feature branches
feature/new-operator      # New language features
fix/parser-error-recovery # Bug fixes
refactor/ir-generation   # Code improvements
docs/api-reference       # Documentation updates
```

### Development Process

1. **Feature Development:**
   ```bash
   # Create feature branch
   git checkout develop
   git pull origin develop
   git checkout -b feature/string-interpolation
   
   # Develop feature with tests
   # - Add lexer tokens
   # - Update parser
   # - Add semantic analysis
   # - Update code generation
   # - Add test cases
   
   # Commit changes
   git add .
   git commit -m "Add string interpolation support
   
   - Add TOKEN_INTERPOLATION to lexer
   - Implement parsing for ${} syntax
   - Add type checking for interpolated expressions
   - Generate appropriate C code
   - Add comprehensive test cases"
   ```

2. **Testing Requirements:**
   ```bash
   # Before committing, ensure all tests pass
   zig build                    # Build must succeed
   ./codex/test_cli.sh quick   # Quick tests must pass
   ./codex/test_cli.sh bulk    # Full test suite must pass
   
   # Add specific tests for new feature
   # Create tests/basics/strings/interpolation.dr
   # Update test configuration if needed
   ```

3. **Code Review Process:**
   ```bash
   # Push feature branch
   git push origin feature/string-interpolation
   
   # Create pull request with:
   # - Clear description of changes
   # - Test results on all platforms
   # - Documentation updates
   # - Breaking change notifications
   ```

### Debugging Workflows

#### Debugging Compilation Issues

```bash
# Enable verbose compilation output
./zig-out/bin/DreamCompiler test.dr -v

# Debug specific compilation phases
./zig-out/bin/DreamCompiler test.dr --debug-lexer
./zig-out/bin/DreamCompiler test.dr --debug-parser
./zig-out/bin/DreamCompiler test.dr --debug-semantic
./zig-out/bin/DreamCompiler test.dr --debug-codegen

# Examine generated C code
cat build/bin/dream.c

# Manual compilation of generated code
gcc -g -O0 build/bin/dream.c src/runtime/memory.c -o debug_executable
```

#### Memory Debugging

```bash
# Linux with Valgrind
valgrind --tool=memcheck --leak-check=full \
  ./zig-out/bin/DreamCompiler test.dr

# Linux with AddressSanitizer
gcc -fsanitize=address -g src/*.c -o dream_debug
./dream_debug test.dr

# Enable internal memory debugging
export DREAM_DEBUG_MEMORY=1
./zig-out/bin/DreamCompiler test.dr
```

#### Performance Profiling

```bash
# Linux with perf
perf record -g ./zig-out/bin/DreamCompiler large_project/
perf report

# Profile compilation time
time ./zig-out/bin/DreamCompiler test.dr

# Profile generated code execution
time ./build/bin/dream

# Memory usage profiling
/usr/bin/time -v ./zig-out/bin/DreamCompiler test.dr
```

---

## Adding Language Features

This section provides step-by-step instructions for adding new language features.

### Example: Adding String Interpolation

String interpolation allows embedding expressions in string literals: `"Hello ${name}!"`

#### Step 1: Lexer Changes

**File**: `src/lexer/lexer.re`

```c
// Add new token types to tokens.def
TOKEN_INTERPOLATION_START,  // "${" 
TOKEN_INTERPOLATION_END,    // "}"

// Add lexer rules
"${"                       { return TOKEN_INTERPOLATION_START; }
```

**File**: `src/lexer/tokens.def`

```c
TOKEN_DEF(INTERPOLATION_START, "${")
TOKEN_DEF(INTERPOLATION_END, "}")
```

Regenerate the lexer:
```bash
re2c -c -o src/lexer/lexer.c src/lexer/lexer.re
```

#### Step 2: AST Changes

**File**: `src/parser/ast.h`

```c
// Add new AST node type
typedef enum {
    // ... existing types ...
    AST_STRING_INTERPOLATION
} NodeType;

// Add interpolation node structure
typedef struct {
    ASTNode** parts;        // String parts and expressions
    int part_count;
    bool* is_expression;    // Track which parts are expressions
} StringInterpolation;

// Add to AST node union
typedef struct ASTNode {
    NodeType type;
    SourceLocation location;
    union {
        // ... existing members ...
        StringInterpolation string_interpolation;
    } data;
} ASTNode;
```

#### Step 3: Parser Changes

**File**: `src/parser/parser.c`

```c
// Add parsing function for string interpolation
ASTNode* parse_string_interpolation(Parser* parser) {
    ASTNode* node = ast_create_node(AST_STRING_INTERPOLATION);
    node->location = parser->current_token->location;
    
    List* parts = list_create();
    List* is_expr = list_create();
    
    // Expect opening quote
    if (!expect_token(parser, TOKEN_STRING)) {
        // Handle error
        return NULL;
    }
    
    char* string_content = parser->previous_token->text;
    int pos = 0;
    int len = strlen(string_content);
    
    while (pos < len) {
        if (pos < len - 1 && string_content[pos] == '$' && 
            string_content[pos + 1] == '{') {
            
            // Found interpolation start
            pos += 2; // Skip "${"
            
            // Parse expression until '}'
            char* expr_text = extract_expression_text(string_content, &pos);
            ASTNode* expr = parse_expression_from_text(expr_text);
            
            list_add(parts, expr);
            list_add(is_expr, (void*)true);
            
            free(expr_text);
        } else {
            // Regular string part
            char* string_part = extract_string_part(string_content, &pos);
            ASTNode* str_node = create_string_literal(string_part);
            
            list_add(parts, str_node);
            list_add(is_expr, (void*)false);
            
            free(string_part);
        }
    }
    
    // Convert lists to arrays
    node->data.string_interpolation.parts = list_to_array(parts);
    node->data.string_interpolation.part_count = list_size(parts);
    node->data.string_interpolation.is_expression = 
        (bool*)list_to_array(is_expr);
    
    list_destroy(parts);
    list_destroy(is_expr);
    
    return node;
}

// Integrate into expression parsing
ASTNode* parse_primary(Parser* parser) {
    switch (parser->current_token->type) {
        // ... existing cases ...
        
        case TOKEN_STRING:
            // Check if string contains interpolation
            if (contains_interpolation(parser->current_token->text)) {
                return parse_string_interpolation(parser);
            } else {
                return parse_string_literal(parser);
            }
            
        // ... rest of cases ...
    }
}
```

#### Step 4: Semantic Analysis

**File**: `src/sem/type_checker.c`

```c
TypeInfo* type_check_string_interpolation(StringInterpolation* interp, 
                                        SymbolTable* scope) {
    // All interpolated expressions must be convertible to string
    for (int i = 0; i < interp->part_count; i++) {
        if (interp->is_expression[i]) {
            TypeInfo* expr_type = type_check_expression(interp->parts[i], scope);
            if (!expr_type) {
                return NULL; // Error in expression
            }
            
            // Check if type can be converted to string
            if (!is_string_convertible(expr_type)) {
                semantic_error("Expression in string interpolation must be "
                             "convertible to string", 
                             interp->parts[i]->location);
                return NULL;
            }
        }
    }
    
    // Result type is always string
    return get_string_type();
}

// Add to main type checking function
TypeInfo* type_check_expression(ASTNode* expr, SymbolTable* scope) {
    switch (expr->type) {
        // ... existing cases ...
        
        case AST_STRING_INTERPOLATION:
            return type_check_string_interpolation(
                &expr->data.string_interpolation, scope);
            
        // ... other cases ...
    }
}
```

#### Step 5: Code Generation

**File**: `src/codegen/c_emitter.c`

```c
void generate_string_interpolation(StringInterpolation* interp, 
                                 CodeGenerator* gen) {
    // Use StringBuilder for efficient string building
    fprintf(gen->output, "({");
    indent_more(gen);
    newline(gen);
    
    // Create StringBuilder
    fprintf(gen->output, "StringBuilder* sb = dr_stringbuilder_create();");
    newline(gen);
    
    // Add each part
    for (int i = 0; i < interp->part_count; i++) {
        if (interp->is_expression[i]) {
            // Convert expression to string and append
            fprintf(gen->output, "dr_stringbuilder_append(sb, ");
            
            // Generate appropriate conversion
            TypeInfo* expr_type = get_expression_type(interp->parts[i]);
            generate_to_string_conversion(interp->parts[i], expr_type, gen);
            
            fprintf(gen->output, ");");
        } else {
            // Append string literal
            fprintf(gen->output, "dr_stringbuilder_append(sb, ");
            generate_expression(interp->parts[i], gen);
            fprintf(gen->output, ");");
        }
        newline(gen);
    }
    
    // Get final string and cleanup
    fprintf(gen->output, "char* result = dr_stringbuilder_tostring(sb);");
    newline(gen);
    fprintf(gen->output, "dr_stringbuilder_destroy(sb);");
    newline(gen);
    fprintf(gen->output, "result;");
    
    indent_less(gen);
    newline(gen);
    fprintf(gen->output, "})");
}

// Helper function for type-specific string conversion
void generate_to_string_conversion(ASTNode* expr, TypeInfo* type, 
                                 CodeGenerator* gen) {
    switch (type->kind) {
        case TYPE_INT:
            fprintf(gen->output, "dr_int_to_string(");
            generate_expression(expr, gen);
            fprintf(gen->output, ")");
            break;
            
        case TYPE_FLOAT:
            fprintf(gen->output, "dr_float_to_string(");
            generate_expression(expr, gen);
            fprintf(gen->output, ")");
            break;
            
        case TYPE_STRING:
            // Already a string
            generate_expression(expr, gen);
            break;
            
        case TYPE_BOOL:
            fprintf(gen->output, "(");
            generate_expression(expr, gen);
            fprintf(gen->output, " ? \"true\" : \"false\")");
            break;
            
        default:
            fprintf(gen->output, "dr_object_to_string(");
            generate_expression(expr, gen);
            fprintf(gen->output, ")");
            break;
    }
}
```

#### Step 6: Runtime Support

**File**: `src/runtime/custom.c`

```c
// Add string conversion functions for different types
char* dr_int_to_string(int value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return dr_alloc_string(buffer);
}

char* dr_float_to_string(double value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.6g", value);
    return dr_alloc_string(buffer);
}

char* dr_object_to_string(void* obj) {
    // Default object string representation
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Object@%p", obj);
    return dr_alloc_string(buffer);
}
```

#### Step 7: Testing

**File**: `tests/basics/strings/interpolation.dr`

```dream
// Test string interpolation functionality
func main() {
    string name = "Alice";
    int age = 30;
    float height = 5.75;
    bool employed = true;
    
    // Basic interpolation
    string greeting = $"Hello, {name}!";
    Console.WriteLine(greeting);
    
    // Multiple expressions
    string info = $"{name} is {age} years old and {height} feet tall";
    Console.WriteLine(info);
    
    // Boolean interpolation
    string status = $"Employment status: {employed}";
    Console.WriteLine(status);
    
    // Arithmetic in interpolation
    string calculation = $"Next year {name} will be {age + 1} years old";
    Console.WriteLine(calculation);
    
    // Method calls in interpolation
    string upper_name = $"HELLO {name.ToUpper()}!";
    Console.WriteLine(upper_name);
}
```

**Expected Output:**
```
Hello, Alice!
Alice is 30 years old and 5.75 feet tall
Employment status: true
Next year Alice will be 31 years old
HELLO ALICE!
```

#### Step 8: Documentation

Update documentation files:

**File**: `docs/language-reference.md`

Add section on string interpolation:

```markdown
### String Interpolation

String interpolation allows embedding expressions directly in string literals using the `${}` syntax:

```dream
string name = "World";
int count = 42;
string message = $"Hello {name}! Count: {count}";
// Result: "Hello World! Count: 42"
```

**Features:**
- Any expression that can be converted to string
- Automatic type conversion for primitives
- Nested expressions supported
- Efficient StringBuilder-based implementation
```

#### Step 9: Build Integration

Update build files to include new test:

**File**: `codex/test_config.json`

```json
{
  "test_categories": {
    "basics": {
      "description": "Core language features",
      "pattern": "tests/basics/**/*.dr",
      "includes": [
        "tests/basics/strings/interpolation.dr"
      ]
    }
  }
}
```

### Testing Your Feature

```bash
# Build compiler with new feature
zig build

# Test the new feature
./zig-out/bin/DreamCompiler tests/basics/strings/interpolation.dr

# Examine generated C code
cat build/bin/dream.c

# Compile and run generated code
gcc build/bin/dream.c src/runtime/memory.c src/runtime/console.c src/runtime/custom.c -o test_interp
./test_interp

# Run full test suite
./codex/test_cli.sh bulk
```

---

## Performance Optimization

### Compiler Performance

#### Compilation Speed Optimization

1. **Parallel Parsing** for multi-file projects:
```c
// Multi-threaded file processing
typedef struct {
    const char* filename;
    ASTNode* result;
    bool success;
} ParseTask;

void* parse_file_thread(void* arg) {
    ParseTask* task = (ParseTask*)arg;
    
    Lexer* lexer = lexer_create_from_file(task->filename);
    Parser* parser = parser_create(lexer);
    
    task->result = parse_program(parser);
    task->success = (task->result != NULL);
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    return NULL;
}

bool parse_files_parallel(const char** filenames, int count, 
                         ASTNode** results) {
    pthread_t* threads = malloc(count * sizeof(pthread_t));
    ParseTask* tasks = malloc(count * sizeof(ParseTask));
    
    // Start parsing threads
    for (int i = 0; i < count; i++) {
        tasks[i].filename = filenames[i];
        pthread_create(&threads[i], NULL, parse_file_thread, &tasks[i]);
    }
    
    // Wait for completion
    bool all_success = true;
    for (int i = 0; i < count; i++) {
        pthread_join(threads[i], NULL);
        results[i] = tasks[i].result;
        all_success = all_success && tasks[i].success;
    }
    
    free(threads);
    free(tasks);
    return all_success;
}
```

2. **Memory Pool Optimization**:
```c
// Custom memory allocator for AST nodes
typedef struct MemoryPool {
    void* memory;
    size_t size;
    size_t used;
    size_t block_size;
} MemoryPool;

MemoryPool* pool_create(size_t total_size, size_t block_size) {
    MemoryPool* pool = malloc(sizeof(MemoryPool));
    pool->memory = malloc(total_size);
    pool->size = total_size;
    pool->used = 0;
    pool->block_size = block_size;
    return pool;
}

void* pool_alloc(MemoryPool* pool) {
    if (pool->used + pool->block_size > pool->size) {
        return NULL; // Pool exhausted
    }
    
    void* ptr = (char*)pool->memory + pool->used;
    pool->used += pool->block_size;
    return ptr;
}

// Use pools for frequently allocated objects
static MemoryPool* ast_node_pool = NULL;

ASTNode* ast_create_node(NodeType type) {
    if (!ast_node_pool) {
        ast_node_pool = pool_create(1024 * 1024, sizeof(ASTNode)); // 1MB pool
    }
    
    ASTNode* node = (ASTNode*)pool_alloc(ast_node_pool);
    if (!node) {
        // Fall back to regular malloc
        node = malloc(sizeof(ASTNode));
    }
    
    node->type = type;
    return node;
}
```

#### Generated Code Performance

1. **Optimization Pipeline**:
```c
// Comprehensive optimization pipeline
void run_optimization_pipeline(CFG* cfg) {
    bool changed;
    int iteration = 0;
    
    do {
        changed = false;
        iteration++;
        
        // Constant propagation and folding
        changed |= constant_propagation_pass(cfg);
        changed |= constant_folding_pass(cfg);
        
        // Dead code elimination
        changed |= dead_code_elimination_pass(cfg);
        
        // Common subexpression elimination
        changed |= common_subexpr_elimination_pass(cfg);
        
        // Loop optimizations
        changed |= loop_invariant_code_motion_pass(cfg);
        
        // Final cleanup
        changed |= simplify_cfg_pass(cfg);
        
    } while (changed && iteration < 10); // Limit iterations
}
```

2. **Efficient String Handling**:
```c
// String interning for reduced memory usage
typedef struct StringInterner {
    HashMap* strings;
    char** interned_strings;
    int count;
    int capacity;
} StringInterner;

const char* intern_string(StringInterner* interner, const char* str) {
    // Check if already interned
    const char* existing = hashmap_get(interner->strings, str);
    if (existing) {
        return existing;
    }
    
    // Add to intern table
    char* interned = strdup(str);
    hashmap_put(interner->strings, interned, interned);
    
    // Track for cleanup
    if (interner->count >= interner->capacity) {
        interner->capacity *= 2;
        interner->interned_strings = realloc(interner->interned_strings,
                                           interner->capacity * sizeof(char*));
    }
    
    interner->interned_strings[interner->count++] = interned;
    return interned;
}
```

### Profiling and Measurement

#### Compilation Performance Measurement

```bash
# Create performance test script
#!/bin/bash
# performance_test.sh

echo "Compilation Performance Test"
echo "==========================="

# Test different project sizes
for size in small medium large; do
    echo "Testing $size project..."
    
    start_time=$(date +%s.%N)
    ./zig-out/bin/DreamCompiler test_projects/$size/ --multi-file
    end_time=$(date +%s.%N)
    
    duration=$(echo "$end_time - $start_time" | bc)
    echo "  Compilation time: ${duration}s"
    
    # Measure memory usage
    /usr/bin/time -v ./zig-out/bin/DreamCompiler test_projects/$size/ --multi-file 2>&1 | \
        grep "Maximum resident set size" | \
        awk '{print "  Peak memory: " $6 " KB"}'
done
```

#### Runtime Performance Benchmarks

```dream
// Benchmark test file (tests/performance/benchmarks.dr)
func fibonacci_recursive(int n) -> int {
    if (n <= 1) return n;
    return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2);
}

func fibonacci_iterative(int n) -> int {
    if (n <= 1) return n;
    
    int prev = 0;
    int curr = 1;
    
    for (int i = 2; i <= n; i++) {
        int next = prev + curr;
        prev = curr;
        curr = next;
    }
    
    return curr;
}

func string_concatenation_test(int iterations) {
    string result = "";
    for (int i = 0; i < iterations; i++) {
        result = result + "x";
    }
    return result.Length;
}

func stringbuilder_test(int iterations) {
    StringBuilder sb = new StringBuilder();
    for (int i = 0; i < iterations; i++) {
        sb.Append("x");
    }
    return sb.Length;
}

func main() {
    // Measure different algorithms
    DateTime start = DateTime.Now;
    
    int fib_result = fibonacci_iterative(35);
    
    DateTime end = DateTime.Now;
    TimeSpan duration = end.Subtract(start);
    
    Console.WriteLine($"Fibonacci(35) = {fib_result}");
    Console.WriteLine($"Time: {duration.TotalMilliseconds}ms");
}
```

---

## Contributing Guidelines

### Code Quality Standards

#### Static Analysis Integration

```bash
# Add static analysis tools to build process
#!/bin/bash
# scripts/static_analysis.sh

echo "Running static analysis..."

# Clang static analyzer
scan-build -o static_analysis_results gcc src/*.c

# Cppcheck
cppcheck --enable=all --std=c11 src/

# Custom linting rules
python3 scripts/lint_checker.py src/

echo "Static analysis complete. Check results in static_analysis_results/"
```

#### Code Coverage Measurement

```bash
# Enable code coverage during testing
#!/bin/bash
# scripts/coverage_test.sh

# Build with coverage instrumentation
gcc -fprofile-arcs -ftest-coverage -O0 -g src/*.c -o dream_coverage

# Run all tests
./codex/test_cli.sh bulk

# Generate coverage report
gcov src/*.c
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html

echo "Coverage report generated in coverage_html/"
```

### Pull Request Process

1. **Pre-submission Checklist:**
   ```markdown
   - [ ] All tests pass on Linux and Windows
   - [ ] New features include comprehensive tests
   - [ ] Documentation updated for user-facing changes
   - [ ] Code follows project style guidelines
   - [ ] No memory leaks detected in Valgrind
   - [ ] Performance regression tests pass
   - [ ] Breaking changes documented in PR description
   ```

2. **Commit Message Format:**
   ```
   [category]: Short description (50 chars max)
   
   Longer description explaining the change, its motivation,
   and any side effects. Wrap at 72 characters.
   
   - Bullet points for multiple changes
   - Reference issues: Fixes #123, Closes #456
   - Breaking changes: BREAKING CHANGE: description
   ```

3. **Review Criteria:**
   - Code correctness and safety
   - Performance implications
   - Test coverage adequacy
   - Documentation completeness
   - API design consistency

### Release Process

#### Version Numbering

Dream follows semantic versioning (MAJOR.MINOR.PATCH):

- **MAJOR**: Breaking language changes
- **MINOR**: New features, backward compatible
- **PATCH**: Bug fixes, no new features

#### Release Checklist

```markdown
## Pre-release (v1.1.13)

- [ ] All tests pass on all platforms (Linux: 95%+, Windows: 85%+)
- [ ] Performance benchmarks within acceptable range
- [ ] Documentation updated (changelog, API reference)
- [ ] Version numbers updated in all files
- [ ] Release notes drafted
- [ ] Security audit completed (if applicable)

## Release Day

- [ ] Create release branch: `release/v1.1.13`
- [ ] Final testing on clean environment
- [ ] Tag release: `git tag -a v1.1.13 -m "Release v1.1.13"`
- [ ] Build release artifacts for all platforms
- [ ] Publish release notes
- [ ] Update main branch
- [ ] Announce release

## Post-release

- [ ] Monitor for critical issues
- [ ] Update development environment setup docs
- [ ] Plan next release features
```

This comprehensive developer guide provides everything needed to understand, extend, and contribute to the Dream Compiler project. It covers the complete development lifecycle from initial setup to release management.