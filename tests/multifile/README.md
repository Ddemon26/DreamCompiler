# Multi-File Compilation Test

This directory contains test files for the DreamCompiler's multi-file compilation functionality.

## Test Files

- **`math_utils.dr`** - Math utility module with exported functions
- **`string_utils.dr`** - String utility module with exported functions  
- **`main.dr`** - Main program that imports and uses both utility modules

## Features Demonstrated

1. **Module Declarations** - `module module_name;`
2. **Export Declarations** - `export func ...`
3. **Import Statements** - `import module_name;`
4. **Cross-Module Function Calls** - `module_name.function()`

## Expected Compilation Flow

1. **Dependency Resolution** - Determine compilation order based on imports
2. **Module Compilation** - Generate separate C files for each module
3. **Header Generation** - Create `.h` files with extern declarations for exports
4. **Linking** - Combine all modules with runtime library into final executable

## Usage

```bash
# Compile all modules (future command)
zig build compile-multi tests/multifile/*.dr

# Or individually
./zig-out/bin/DreamCompiler --multi-file tests/multifile/main.dr
```

## Generated Output Structure

```
build/multifile/
├── math_utils.c        # Generated C code
├── math_utils.h        # Export declarations
├── string_utils.c      # Generated C code
├── string_utils.h      # Export declarations
├── main.c              # Generated C code
└── main                # Final executable
```