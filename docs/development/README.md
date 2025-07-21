# Dream Compiler Development

This section contains comprehensive resources for developers working on the Dream compiler, contributing to the project, or seeking to understand the compiler's internal architecture.

## Contents

### [Developer Guide](developer-guide.md)
Complete guide for Dream compiler development:

#### Architecture & Design
- **Compiler Pipeline** - Lexical analysis, parsing, semantic analysis, optimization, code generation
- **Source Code Organization** - Directory structure and coding standards
- **Technology Stack** - Zig build system, C11 implementation, re2c lexer generation

#### Development Environment
- **Setup Instructions** - IDE configuration, required tools, build environment
- **Development Workflow** - Git branching, testing, code review process
- **Debugging Tools** - Compiler debugging, memory analysis, performance profiling

#### Contributing to Dream
- **Adding Language Features** - Step-by-step guide with complete example
- **Performance Optimization** - Compilation speed and generated code quality
- **Testing Requirements** - Unit tests, integration tests, platform validation

### Compiler Implementation Details

#### [Compiler Architecture](compiler/index.md)
Internal design and structure of the Dream compiler:
- **Multi-phase Architecture** - Clear separation of compilation phases
- **AST Design** - Abstract Syntax Tree structure and manipulation
- **Symbol Table Management** - Scope resolution and type information
- **Error Recovery** - Robust error handling and user-friendly messages

#### [Build System](compiler/build.md)  
Comprehensive build system documentation:
- **Zig Build Configuration** - Cross-platform compilation setup
- **Source File Organization** - How compiler sources are structured
- **Multi-file Compilation** - Building complex Dream projects
- **Cross-platform Support** - Windows, Linux, macOS compilation
- **Performance Optimization** - Build speed and incremental compilation

#### [Testing Framework](compiler/testing.md)
Complete testing infrastructure:
- **Test Organization** - 161 tests across multiple categories
- **Cross-platform Test Runners** - Shell, PowerShell, and Python scripts
- **Continuous Integration** - GitHub Actions workflow
- **Performance Testing** - Compilation speed and runtime benchmarks
- **Regression Testing** - Preventing feature regressions

### Development Workflows

#### Contributing Code
1. **Environment Setup** - Development tools and configuration
2. **Feature Development** - Branch strategy and implementation process
3. **Testing Requirements** - All tests must pass before submission
4. **Code Review** - Quality standards and review criteria
5. **Documentation Updates** - Keeping docs in sync with code changes

#### Debugging the Compiler
- **Compilation Phases** - Debug individual compiler phases
- **Memory Debugging** - Valgrind, AddressSanitizer integration
- **Performance Profiling** - Identify compilation bottlenecks
- **Generated Code Analysis** - Verify C code output quality

## Quick Start for Contributors

### Setting Up Development Environment
```bash
# Clone and build
git clone https://github.com/company/DreamCompiler.git
cd DreamCompiler
zig build

# Run quick tests to verify setup
./codex/test_cli.sh quick  # Linux/macOS
.\codex\test_cli.ps1 quick # Windows

# Generate lexer (if modifying lexer.re)
re2c -c -o src/lexer/lexer.c src/lexer/lexer.re
```

### Development Cycle
```bash
# Create feature branch
git checkout -b feature/new-language-feature

# Make changes, following coding standards
# - Update lexer if needed (src/lexer/lexer.re)
# - Modify parser (src/parser/)  
# - Add semantic analysis (src/sem/)
# - Update code generation (src/codegen/)
# - Add comprehensive tests

# Verify changes
zig build                    # Build must succeed
./codex/test_cli.sh bulk    # All tests must pass

# Submit for review
git push origin feature/new-language-feature
# Create pull request with detailed description
```

## Architecture Overview

```
┌─────────────────────────────────────────────────────────┐
│                Dream Compiler Architecture              │  
├─────────────────────────────────────────────────────────┤
│  Source Code (.dr files)                               │
│              ↓                                          │
│  Lexer (re2c) → Tokens                                 │
│              ↓                                          │ 
│  Parser (Pratt) → AST                                  │
│              ↓                                          │
│  Semantic Analyzer → Typed AST                         │
│              ↓                                          │
│  IR Generator → SSA IR                                  │
│              ↓                                          │
│  Optimizer → Optimized IR                              │
│              ↓                                          │
│  Code Generator → C Code                               │
│              ↓                                          │
│  Zig CC → Native Executable                            │
└─────────────────────────────────────────────────────────┘
```

## Key Technologies

### Core Implementation
- **Language**: C11 for maximum portability
- **Build System**: Zig for fast cross-platform builds  
- **Lexer Generator**: re2c for high-performance tokenization
- **Parser**: Hand-written Pratt parser for excellent error recovery

### Development Tools  
- **Version Control**: Git with feature branch workflow
- **CI/CD**: GitHub Actions for automated testing
- **Memory Safety**: Valgrind and AddressSanitizer integration
- **Performance**: Built-in profiling and benchmarking

### Platform Support
- **Primary**: Linux (95% test pass rate)
- **Stable**: Windows (85% test pass rate)  
- **Development**: macOS (in progress)

## Contribution Areas

### Language Features
- **New Syntax** - Extending the language with new constructs
- **Type System** - Generic types, advanced type inference
- **Standard Library** - Built-in classes and functions
- **Optimization** - Improving generated code quality

### Compiler Infrastructure  
- **Error Messages** - Improving diagnostic quality
- **IDE Integration** - Language server protocol implementation
- **Build Performance** - Faster compilation and incremental builds
- **Platform Support** - macOS completion, additional platforms

### Testing & Quality
- **Test Coverage** - Expanding the test suite
- **Performance Tests** - Benchmarking and regression detection  
- **Documentation** - Keeping docs comprehensive and up-to-date
- **Bug Fixes** - Resolving issues and edge cases

## Performance Characteristics

### Compilation Speed
- **Small Projects** (< 1K lines): < 1 second
- **Medium Projects** (1K-10K lines): 2-5 seconds  
- **Large Projects** (10K+ lines): 10-30 seconds
- **Incremental Builds**: 50-80% faster than full rebuild

### Generated Code Quality
- **Optimization**: Multiple optimization passes (SCCP, DCE, LICM, CSE)
- **Memory Efficiency**: Reference counting with cycle detection
- **Runtime Performance**: Competitive with hand-written C
- **Binary Size**: Compact executables with minimal runtime overhead

## Quality Standards

### Code Quality
- **Memory Safety**: No memory leaks, proper resource cleanup
- **Error Handling**: Comprehensive error checking and reporting
- **Documentation**: All public functions documented with examples
- **Testing**: New features require comprehensive test coverage

### Performance Requirements
- **Compilation Speed**: No more than 20% regression
- **Memory Usage**: Compiler memory usage must be reasonable
- **Generated Code**: Performance must match or exceed previous versions
- **Platform Parity**: New features must work on all supported platforms

## What's Next?

### For New Contributors
1. **Read the [Developer Guide](developer-guide.md)** - Comprehensive development guide
2. **Set up development environment** - Tools and configuration
3. **Run the test suite** - Verify everything works
4. **Pick a beginner-friendly issue** - Start with documentation or small fixes
5. **Join the development community** - Collaborate with other contributors

### For Experienced Developers
1. **Choose a focus area** - Language features, optimization, or infrastructure
2. **Review the architecture** - Understand compiler internals
3. **Identify improvement opportunities** - Performance, features, or quality
4. **Design significant changes** - Document proposals and get feedback
5. **Implement and test thoroughly** - Follow development best practices

The Dream compiler is designed for extensibility and maintainability. Contributors can make meaningful improvements while maintaining the high quality and performance standards that make Dream a professional development tool.