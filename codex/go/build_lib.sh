#!/bin/bash
# Build script for DreamCompiler C library for Go testing
# This script handles the complex build process including lexer generation

set -e

# Configuration
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
GO_TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$ROOT_DIR/src"
LEXER_DIR="$SRC_DIR/lexer"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check dependencies
check_dependencies() {
    local missing_deps=()
    
    for tool in zig re2c; do
        if ! command -v "$tool" &> /dev/null; then
            missing_deps+=("$tool")
        fi
    done
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        log_error "Missing dependencies: ${missing_deps[*]}"
        log_info "Please install missing tools:"
        log_info "  - zig: https://ziglang.org/download/"
        log_info "  - re2c: https://re2c.org/"
        exit 1
    fi
}

# Generate lexer if needed
generate_lexer() {
    log_info "Checking lexer generation..."
    
    local lexer_re="$LEXER_DIR/lexer.re"
    local tokens_def="$LEXER_DIR/tokens.def"
    local lexer_c="$LEXER_DIR/lexer.c"
    
    if [ ! -f "$lexer_re" ] || [ ! -f "$tokens_def" ]; then
        log_error "Lexer source files not found:"
        log_error "  Expected: $lexer_re"
        log_error "  Expected: $tokens_def"
        exit 1
    fi
    
    # Check if lexer.c needs regeneration
    if [ ! -f "$lexer_c" ] || [ "$lexer_re" -nt "$lexer_c" ] || [ "$tokens_def" -nt "$lexer_c" ]; then
        log_info "Generating lexer with re2c..."
        cd "$LEXER_DIR"
        re2c -c -o lexer.c lexer.re
        
        if [ $? -eq 0 ]; then
            log_success "Lexer generated successfully"
        else
            log_error "Lexer generation failed"
            exit 1
        fi
    else
        log_info "Lexer is up to date"
    fi
}

# Build the C library
build_library() {
    log_info "Building DreamCompiler C library..."
    
    cd "$GO_TEST_DIR"
    
    # Determine library name based on platform
    local lib_name="libdream.so"
    local lib_flags="-shared"
    
    if [[ "$OSTYPE" == "darwin"* ]]; then
        lib_name="libdream.dylib"
        lib_flags="-shared -undefined dynamic_lookup"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        lib_name="libdream.dll"
        lib_flags="-shared -Wl,--out-implib,libdream.lib"
    fi
    
    # Compiler flags
    local cflags="-std=c11 -Wall -Wextra -D_GNU_SOURCE -fPIC -O2"
    local includes="-I. -I$SRC_DIR -I$LEXER_DIR"
    
    # Source files
    local sources=(
        "$SRC_DIR/parser/ast.c"
        "$SRC_DIR/parser/parser.c"
        "$SRC_DIR/parser/error.c"
        "$SRC_DIR/parser/diagnostic.c"
        "$SRC_DIR/parser/warnings.c"
        "$SRC_DIR/sem/scope.c"
        "$SRC_DIR/sem/symbol.c"
        "$SRC_DIR/sem/type.c"
        "$SRC_DIR/sem/infer.c"
        "$SRC_DIR/sem/analysis.c"
        "$SRC_DIR/ir/ir.c"
        "$SRC_DIR/ir/lower.c"
        "$SRC_DIR/cfg/cfg.c"
        "$SRC_DIR/ssa/ssa.c"
        "$SRC_DIR/opt/pipeline.c"
        "$SRC_DIR/opt/sccp.c"
        "$SRC_DIR/opt/dce.c"
        "$SRC_DIR/opt/value_numbering.c"
        "$SRC_DIR/opt/licm.c"
        "$SRC_DIR/opt/copy_prop.c"
        "$SRC_DIR/opt/cse.c"
        "$SRC_DIR/opt/peephole.c"
        "$SRC_DIR/opt/inline.c"
        "$SRC_DIR/opt/loop_opt.c"
        "$SRC_DIR/codegen/c_emit.c"
        "$SRC_DIR/codegen/context.c"
        "$SRC_DIR/codegen/expr.c"
        "$SRC_DIR/codegen/stmt.c"
        "$SRC_DIR/codegen/codegen.c"
        "$SRC_DIR/codegen/backend.c"
        "$SRC_DIR/codegen/module.c"
        "$LEXER_DIR/lexer.c"
        "dream_api.c"
    )
    
    # Check that all source files exist
    for src in "${sources[@]}"; do
        if [ ! -f "$src" ]; then
            log_error "Source file not found: $src"
            exit 1
        fi
    done
    
    # Build command
    local build_cmd="zig cc $cflags $includes $lib_flags -o $lib_name ${sources[*]}"
    
    log_info "Build command: $build_cmd"
    
    # Execute build
    if eval "$build_cmd"; then
        log_success "Library built successfully: $lib_name"
        
        # Verify the library
        if [ -f "$lib_name" ]; then
            local size=$(stat -f%z "$lib_name" 2>/dev/null || stat -c%s "$lib_name" 2>/dev/null || echo "unknown")
            log_info "Library size: $size bytes"
            
            # Show library info if available
            if command -v file &> /dev/null; then
                log_info "Library info: $(file "$lib_name")"
            fi
        else
            log_error "Library file not created: $lib_name"
            exit 1
        fi
    else
        log_error "Library build failed"
        exit 1
    fi
}

# Verify Go environment
verify_go() {
    log_info "Verifying Go environment..."
    
    if ! command -v go &> /dev/null; then
        log_error "Go not found. Please install Go 1.21 or later."
        exit 1
    fi
    
    local go_version=$(go version | grep -o 'go[0-9]\+\.[0-9]\+' | head -1)
    log_info "Go version: $go_version"
    
    # Check if go.mod exists
    if [ ! -f "go.mod" ]; then
        log_warning "go.mod not found, initializing Go module..."
        go mod init dreamcompiler/tests
        go mod tidy
    fi
    
    log_success "Go environment verified"
}

# Run a quick test to verify the build
test_build() {
    log_info "Testing the built library..."
    
    # Try to run a simple Go test that uses the library
    if go test -run TestLexer_BasicTokens -v . &> /dev/null; then
        log_success "Library test passed"
    else
        log_warning "Library test failed, but library was built"
        log_info "You may need to run 'go mod tidy' or check cgo configuration"
    fi
}

# Main function
main() {
    log_info "DreamCompiler C Library Build Script"
    log_info "Root directory: $ROOT_DIR"
    log_info "Go test directory: $GO_TEST_DIR"
    
    check_dependencies
    generate_lexer
    verify_go
    build_library
    test_build
    
    log_success "Build completed successfully!"
    log_info "You can now run Go tests with: go test -v ./..."
}

# Handle command line arguments
case "${1:-}" in
    "clean")
        log_info "Cleaning build artifacts..."
        cd "$GO_TEST_DIR"
        rm -f libdream.so libdream.dylib libdream.dll libdream.lib
        go clean -testcache
        log_success "Clean completed"
        ;;
    "help"|"-h"|"--help")
        echo "DreamCompiler C Library Build Script"
        echo ""
        echo "Usage: $0 [command]"
        echo ""
        echo "Commands:"
        echo "  (none)    Build the library (default)"
        echo "  clean     Clean build artifacts"
        echo "  help      Show this help"
        echo ""
        echo "This script:"
        echo "  1. Checks for required dependencies (zig, re2c)"
        echo "  2. Generates the lexer if needed"
        echo "  3. Verifies Go environment"
        echo "  4. Builds the C library for Go testing"
        echo "  5. Runs a quick verification test"
        ;;
    *)
        main
        ;;
esac