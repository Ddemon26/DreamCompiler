# Dream Compiler Architecture

The DreamCompiler is a sophisticated, multi-phase compiler that transforms Dream language source code into optimized C code. This document covers the internal architecture, build system, and development workflows.

## Compilation Pipeline

The compiler follows a traditional multi-phase architecture:

```
Source (.dr) → Lexer → Parser → Semantic Analysis → IR → Optimization → Code Generation → C Output
```

### Phase 1: Lexical Analysis (`src/lexer/`)
- **Technology**: re2c-generated lexer for high performance
- **Input**: Raw `.dr` source files
- **Output**: Token stream
- **Features**: Unicode support, comprehensive error recovery, position tracking

```c
// Token definitions in src/lexer/tokens.def
TOKEN_INT     // Integer literals: 42, 0xFF, 0b1010
TOKEN_FLOAT   // Float literals: 3.14, 1e10
TOKEN_STRING  // String literals: "hello"
TOKEN_IDENT   // Identifiers: variable_name
```

### Phase 2: Parsing (`src/parser/`)
- **Technology**: Pratt parser (recursive descent with precedence climbing)
- **Input**: Token stream
- **Output**: Abstract Syntax Tree (AST)
- **Features**: Excellent error recovery, operator precedence handling

```c
// AST node types
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION,
    AST_VARIABLE_DECL,
    AST_BINARY_OP,
    AST_CALL_EXPR,
    // ... more node types
} ast_node_type_t;
```

### Phase 3: Semantic Analysis (`src/sem/`)
- **Type Checking**: Static type verification with inference
- **Symbol Resolution**: Variable and function scope analysis  
- **Error Detection**: Comprehensive semantic error reporting

```c
// Type system
typedef enum {
    TYPE_INT,
    TYPE_FLOAT, 
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_CLASS,
    TYPE_VOID
} dream_type_t;
```

### Phase 4: IR Generation (`src/ir/`, `src/ssa/`, `src/cfg/`)
- **Three-Address Code**: Simplified instruction format
- **SSA Form**: Static Single Assignment for optimization
- **Control Flow Graph**: Basic block analysis

```c
// IR instruction example
struct ir_instruction {
    ir_opcode_t opcode;     // ADD, SUB, LOAD, STORE, etc.
    ir_operand_t dest;      // Destination
    ir_operand_t src1;      // Source operand 1  
    ir_operand_t src2;      // Source operand 2
};
```

### Phase 5: Optimization (`src/opt/`)
Multiple optimization passes improve generated code quality:

- **SCCP** - Sparse Conditional Constant Propagation
- **DCE** - Dead Code Elimination
- **LICM** - Loop Invariant Code Motion
- **CSE** - Common Subexpression Elimination
- **Register Allocation** - Efficient variable placement

### Phase 6: Code Generation (`src/codegen/`)
- **Backend Abstraction**: Pluggable code generation system
- **C Code Emission**: Clean, readable C output
- **Runtime Integration**: Automatic linking with Dream runtime

```c
// Generated C code example
#include "memory.h"
#include "console.h"

int dream_main() {
    dr_string_t* name = dr_console_readline();
    dr_string_t* greeting = dr_string_concat("Hello, ", name);
    dr_console_writeline(greeting);
    dr_release_all();
    return 0;
}
```

## Build System

### Zig Build Integration
The compiler uses Zig as its build system for cross-platform compatibility:

```zig
// build.zig - Core compiler sources
const AllCSources = [_][]const u8{
    "src/driver/main.c",
    "src/lexer/lexer.c",
    "src/parser/parser.c", 
    "src/sem/semantic.c",
    "src/ir/ir_gen.c",
    "src/opt/optimizer.c",
    "src/codegen/codegen.c",
    // Runtime components
    "src/runtime/memory.c",
    "src/runtime/console.c",
    "src/runtime/custom.c"
};
```

### Build Commands
```bash
# Build compiler and runtime
zig build

# Compile a Dream program
zig build run -- program.dr

# Multi-file compilation
zig build compile-multi

# Development builds with debug info
zig build -Doptimize=Debug
```

## Runtime System

### Memory Management (`src/runtime/memory.h/.c`)
Dream uses reference-counted memory management:

```c
// Core memory management API
void* dr_alloc(size_t size);
void dr_retain(void* ptr);
void dr_release(void* ptr);
void dr_release_all(void);  // Called at program end
```

### Console I/O (`src/runtime/console.h/.c`)
Built-in console operations:

```c
// Console API
void dr_console_write(dr_string_t* str);
void dr_console_writeline(dr_string_t* str);
dr_string_t* dr_console_readline(void);
```

### Runtime Library Distribution
The build system creates `libdreamrt.a` containing all runtime components:

```bash
# Runtime library contents
build/libs/
├── libdreamrt.a        # Static library
├── memory.h            # Memory management headers
├── console.h           # Console I/O headers  
└── custom.h            # Custom runtime headers
```

## Multi-File Compilation

### Module System Architecture
The compiler supports modular compilation with dependency resolution:

```c
// Module manager (src/codegen/module.h)
typedef struct {
    char* name;
    char** dependencies;
    char* output_path;
    bool compiled;
} dream_module_t;
```

### Backend Abstraction
Pluggable code generation supports multiple targets:

```c
// Backend interface (src/codegen/backend.h)
typedef struct {
    void (*emit_function)(function_t* func);
    void (*emit_class)(class_t* cls);
    void (*emit_module)(module_t* mod);
} backend_interface_t;
```

## Testing Framework

### Test Organization
```bash
tests/
├── basics/           # Core language features (75+ tests)
├── control_flow/     # Conditionals, loops (25+ tests)
├── functions/        # Function definitions (10+ tests)
├── advanced/         # OOP, data structures (15+ tests)
├── multifile/        # Multi-file compilation (3 files)
└── semantics/        # Type checking tests
```

### Test Execution
```bash
# Fast testing (< 5 seconds)
./codex/test_cli.sh quick

# Category-specific testing
./codex/test_cli.sh filter basics
./codex/test_cli.sh filter advanced

# Full test suite (~30 seconds)
./codex/test_cli.sh bulk

# Cross-platform testing
python codex/test_cli.py run --pattern "basics/**/*.dr"
```

### Go API Testing
```bash
cd codex/go
make test     # Unit tests via cgo
make bench    # Performance benchmarks  
make fuzz     # Fuzz testing
```

## Debug Support

### Source Line Mapping
Generated C code includes `#line` directives for debugging:

```c
// Generated with source mapping
#line 15 "program.dr"
int x = 42;
#line 16 "program.dr"  
printf("x = %d\n", x);
```

### Debug Builds
```bash
# Enable debug symbols
zig build -Doptimize=Debug

# Compile with debugging
gcc -g dream.c memory.c -o dream
gdb ./dream  # Source-level debugging
```

## Performance Characteristics

### Compilation Speed
- **Lexing**: ~1M tokens/second
- **Parsing**: ~500K nodes/second  
- **Full Compilation**: ~10K lines/second

### Memory Usage
- **Compiler**: ~50MB peak memory for large projects
- **Runtime**: Reference counting overhead ~10-15%

### Generated Code Quality
- **Optimization**: Comparable to `-O2` C compilation
- **Binary Size**: ~20% overhead vs hand-written C
- **Runtime Performance**: 90-95% of equivalent C code

## Development Workflow

### Grammar Updates
1. Update `docs/grammar/Grammar.md` (authoritative source)
2. Modify `src/lexer/tokens.def` for new tokens
3. Regenerate lexer: `re2c -c -o src/lexer/lexer.c src/lexer/lexer.re`
4. Update parser in `src/parser/` for new syntax
5. Add tests in appropriate `tests/` category

### Contribution Guidelines
- All changes must pass the full test suite
- New features require comprehensive test coverage
- Documentation must be updated for user-facing changes
- Cross-platform compatibility must be maintained

## IDE Integration

See [IDE Integration](intellisense.md) for setup instructions for:
- Visual Studio Code
- CLion  
- Vim/Neovim
- Language Server Protocol implementation
