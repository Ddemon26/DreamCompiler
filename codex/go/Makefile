# Makefile for DreamCompiler Go Tests
# Builds the C library and runs Go tests

# Configuration
CC = zig cc
CFLAGS = -std=c11 -Wall -Wextra -D_GNU_SOURCE -fPIC -O2
LDFLAGS = -shared
INCLUDES = -I. -I../src -I../src/lexer

# Source files (relative to codex directory)
C_SOURCES = \
	../src/parser/ast.c \
	../src/parser/parser.c \
	../src/parser/error.c \
	../src/parser/diagnostic.c \
	../src/sem/scope.c \
	../src/sem/symbol.c \
	../src/sem/type.c \
	../src/sem/infer.c \
	../src/sem/analysis.c \
	../src/ir/ir.c \
	../src/ir/lower.c \
	../src/cfg/cfg.c \
	../src/ssa/ssa.c \
	../src/opt/pipeline.c \
	../src/opt/sccp.c \
	../src/opt/dce.c \
	../src/opt/value_numbering.c \
	../src/opt/licm.c \
	../src/opt/copy_prop.c \
	../src/opt/cse.c \
	../src/opt/peephole.c \
	../src/codegen/c_emit.c \
	../src/codegen/context.c \
	../src/codegen/expr.c \
	../src/codegen/stmt.c \
	../src/codegen/codegen.c \
	dream_api.c

# Generated lexer source
LEXER_SOURCE = ../src/lexer/lexer.c

# Output library
LIBRARY = libdream.so
ifeq ($(OS),Windows_NT)
	LIBRARY = libdream.dll
	LDFLAGS = -shared -Wl,--out-implib,libdream.lib
endif

# Default target
.PHONY: all
all: build test

# Build the C library
.PHONY: build
build: $(LIBRARY)

$(LIBRARY): $(C_SOURCES) $(LEXER_SOURCE) dream_api.c
	@echo "Building DreamCompiler C library..."
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^
	@echo "Library built: $(LIBRARY)"

# Generate lexer if needed
$(LEXER_SOURCE): ../src/lexer/lexer.re ../src/lexer/tokens.def
	@echo "Generating lexer..."
	cd ../src/lexer && re2c -c -o lexer.c lexer.re

# Run Go tests
.PHONY: test
test: $(LIBRARY)
	@echo "Running Go tests..."
	go test -v ./...

# Run Go tests with coverage
.PHONY: test-coverage
test-coverage: $(LIBRARY)
	@echo "Running Go tests with coverage..."
	go test -v -cover ./...

# Run benchmarks
.PHONY: bench
bench: $(LIBRARY)
	@echo "Running Go benchmarks..."
	go test -v -bench=. -benchmem ./...

# Run fuzz tests (for a short duration)
.PHONY: fuzz
fuzz: $(LIBRARY)
	@echo "Running fuzz tests..."
	go test -fuzz=FuzzLexer -fuzztime=30s ./...
	go test -fuzz=FuzzParser -fuzztime=30s ./...
	go test -fuzz=FuzzCodegen -fuzztime=30s ./...

# Run all tests including fuzz
.PHONY: test-all
test-all: $(LIBRARY)
	@echo "Running all tests..."
	go test -v ./...
	go test -v -bench=. ./...
	go test -fuzz=FuzzLexer -fuzztime=10s ./...
	go test -fuzz=FuzzParser -fuzztime=10s ./...

# Clean build artifacts
.PHONY: clean
clean:
	rm -f $(LIBRARY) libdream.lib
	go clean -testcache
	@echo "Cleaned build artifacts"

# Install Go dependencies
.PHONY: deps
deps:
	@echo "Installing Go dependencies..."
	go mod tidy
	go mod download

# Verify the build works
.PHONY: verify
verify: build
	@echo "Verifying build..."
	@if [ -f "$(LIBRARY)" ]; then \
		echo "✓ Library built successfully: $(LIBRARY)"; \
		file $(LIBRARY) 2>/dev/null || echo "Library file exists"; \
	else \
		echo "✗ Library build failed"; \
		exit 1; \
	fi

# Development workflow
.PHONY: dev
dev: clean build test

# CI workflow
.PHONY: ci
ci: deps verify test bench

# Help target
.PHONY: help
help:
	@echo "DreamCompiler Go Tests Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build library and run tests (default)"
	@echo "  build        - Build the C library"
	@echo "  test         - Run Go unit tests"
	@echo "  test-coverage- Run tests with coverage"
	@echo "  bench        - Run benchmarks"
	@echo "  fuzz         - Run fuzz tests (short duration)"
	@echo "  test-all     - Run all tests including fuzz"
	@echo "  clean        - Clean build artifacts"
	@echo "  deps         - Install Go dependencies"
	@echo "  verify       - Verify the build"
	@echo "  dev          - Development workflow (clean + build + test)"
	@echo "  ci           - CI workflow (deps + verify + test + bench)"
	@echo "  help         - Show this help"
	@echo ""
	@echo "Environment Variables:"
	@echo "  CC           - C compiler (default: zig cc)"
	@echo "  CFLAGS       - C compiler flags"
	@echo "  LDFLAGS      - Linker flags"

# Debug target to show variables
.PHONY: debug
debug:
	@echo "Configuration:"
	@echo "  CC = $(CC)"
	@echo "  CFLAGS = $(CFLAGS)"
	@echo "  LDFLAGS = $(LDFLAGS)"
	@echo "  INCLUDES = $(INCLUDES)"
	@echo "  LIBRARY = $(LIBRARY)"
	@echo "  OS = $(OS)"
	@echo ""
	@echo "Sources:"
	@echo "  C_SOURCES = $(C_SOURCES)"
	@echo "  LEXER_SOURCE = $(LEXER_SOURCE)"