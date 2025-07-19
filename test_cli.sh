#!/bin/bash
# DreamCompiler Test CLI for Linux
# Provides fast test execution and management for CLI agents

set -e

# Configuration
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="$ROOT_DIR/tests"
RESULTS_DIR="$ROOT_DIR/test_results"
PYTHON_CLI="$ROOT_DIR/test_cli.py"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
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
    
    # Check required tools
    for tool in zig python3 make gcc; do
        if ! command -v "$tool" &> /dev/null; then
            missing_deps+=("$tool")
        fi
    done
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        log_error "Missing dependencies: ${missing_deps[*]}"
        log_info "Please install missing tools and try again"
        exit 1
    fi
    
    # Check Python modules
    if ! python3 -c "import json, pathlib, subprocess" &> /dev/null; then
        log_error "Required Python modules not available"
        exit 1
    fi
}

# Build compiler
build_compiler() {
    log_info "Building DreamCompiler..."
    
    if zig build; then
        log_success "Build completed successfully"
        return 0
    else
        log_error "Build failed"
        return 1
    fi
}

# Quick test functions
quick_test() {
    local test_file="$1"
    
    log_info "Running quick test..."
    
    if ! build_compiler; then
        return 1
    fi
    
    if [ -n "$test_file" ]; then
        python3 "$PYTHON_CLI" quick "$test_file"
    else
        python3 "$PYTHON_CLI" quick
    fi
}

# Run test suite
run_tests() {
    local pattern="${1:-**/*.dr}"
    local categories="$2"
    local debug="$3"
    
    log_info "Running test suite with pattern: $pattern"
    
    local cmd="python3 $PYTHON_CLI run --pattern '$pattern'"
    
    if [ -n "$categories" ]; then
        cmd="$cmd --categories $categories"
    fi
    
    if [ "$debug" = "true" ]; then
        cmd="$cmd --debug"
    fi
    
    eval "$cmd"
}

# Filter tests by category
filter_tests() {
    local category="$1"
    local pattern="**/*.dr"
    
    case "$category" in
        "basics"|"basic")
            pattern="basics/**/*.dr"
            ;;
        "advanced")
            pattern="advanced/**/*.dr"
            ;;
        "control"|"control_flow")
            pattern="control_flow/**/*.dr"
            ;;
        "functions"|"func")
            pattern="functions/**/*.dr"
            ;;
        "memory"|"mem")
            pattern="**/memory/*.dr"
            ;;
        "struct"|"structs")
            pattern="**/data_structures/*.dr"
            ;;
        *)
            log_warning "Unknown category: $category"
            ;;
    esac
    
    echo "$pattern"
}

# Bulk test execution
bulk_test() {
    local categories=("basics" "control_flow" "functions" "advanced")
    local results=()
    
    log_info "Running bulk tests across all categories..."
    
    if ! build_compiler; then
        return 1
    fi
    
    for category in "${categories[@]}"; do
        log_info "Testing category: $category"
        
        local pattern
        pattern=$(filter_tests "$category")
        
        if python3 "$PYTHON_CLI" run --pattern "$pattern" > /dev/null 2>&1; then
            results+=("$category:PASS")
            log_success "$category tests passed"
        else
            results+=("$category:FAIL")
            log_error "$category tests failed"
        fi
    done
    
    # Summary
    echo
    log_info "Bulk test summary:"
    for result in "${results[@]}"; do
        local cat="${result%:*}"
        local status="${result#*:}"
        
        if [ "$status" = "PASS" ]; then
            log_success "$cat: PASSED"
        else
            log_error "$cat: FAILED"
        fi
    done
}

# Fast test execution with minimal output
fast_test() {
    local pattern="${1:-basics/**/*.dr}"
    
    log_info "Fast test execution..."
    
    # Build silently
    if ! zig build > /dev/null 2>&1; then
        log_error "Build failed"
        return 1
    fi
    
    # Run tests with minimal output
    local start_time
    start_time=$(date +%s)
    
    if python3 "$PYTHON_CLI" run --pattern "$pattern" > /dev/null 2>&1; then
        local end_time
        end_time=$(date +%s)
        local duration=$((end_time - start_time))
        
        log_success "Tests passed in ${duration}s"
        return 0
    else
        log_error "Tests failed"
        return 1
    fi
}

# Analyze results
analyze_results() {
    local format="${1:-text}"
    local output="$2"
    
    log_info "Analyzing test results..."
    
    local cmd="python3 $PYTHON_CLI analyze --format $format"
    
    if [ -n "$output" ]; then
        cmd="$cmd --output $output"
    fi
    
    eval "$cmd"
}

# List available tests
list_tests() {
    local pattern="${1:-**/*.dr}"
    
    python3 "$PYTHON_CLI" list --pattern "$pattern"
}

# Show environment status
show_status() {
    python3 "$PYTHON_CLI" status
}

# Generate tests
generate_tests() {
    local action="$1"
    
    case "$action" in
        "new"|"generate")
            python3 "$PYTHON_CLI" generate --generate
            ;;
        "validate"|"check")
            python3 "$PYTHON_CLI" generate --validate
            ;;
        "matrix")
            python3 "$PYTHON_CLI" generate --matrix
            ;;
        *)
            log_error "Unknown generate action: $action"
            log_info "Available actions: new, validate, matrix"
            return 1
            ;;
    esac
}

# Watch mode for continuous testing
watch_tests() {
    local pattern="${1:-basics/**/*.dr}"
    
    log_info "Starting watch mode for pattern: $pattern"
    log_info "Press Ctrl+C to stop"
    
    while true; do
        clear
        echo "=== DreamCompiler Test Watch Mode ==="
        echo "Pattern: $pattern"
        echo "Time: $(date)"
        echo
        
        if fast_test "$pattern"; then
            log_success "All tests passing"
        else
            log_error "Some tests failing"
        fi
        
        echo
        echo "Waiting 5 seconds... (Ctrl+C to stop)"
        sleep 5
    done
}

# Performance benchmark
benchmark_tests() {
    local iterations="${1:-3}"
    
    log_info "Running performance benchmark ($iterations iterations)..."
    
    local total_time=0
    local successful_runs=0
    
    for ((i=1; i<=iterations; i++)); do
        log_info "Benchmark run $i/$iterations"
        
        local start_time
        start_time=$(date +%s%N)
        
        if fast_test > /dev/null 2>&1; then
            local end_time
            end_time=$(date +%s%N)
            local duration=$(((end_time - start_time) / 1000000))  # Convert to milliseconds
            
            total_time=$((total_time + duration))
            successful_runs=$((successful_runs + 1))
            
            log_success "Run $i completed in ${duration}ms"
        else
            log_error "Run $i failed"
        fi
    done
    
    if [ $successful_runs -gt 0 ]; then
        local avg_time=$((total_time / successful_runs))
        log_success "Benchmark complete: ${avg_time}ms average (${successful_runs}/${iterations} successful)"
    else
        log_error "All benchmark runs failed"
        return 1
    fi
}

# Help function
show_help() {
    cat << EOF
DreamCompiler Test CLI for Linux

Usage: $0 <command> [options]

Commands:
  quick [file]              Quick test execution (single file or basic suite)
  run [pattern] [debug]     Run test suite with optional pattern and debug mode
  bulk                      Run tests across all categories
  fast [pattern]            Fast test execution with minimal output
  filter <category>         Run tests for specific category
  analyze [format] [output] Analyze test results (text/html/charts)
  list [pattern]            List available tests
  status                    Show environment status
  generate <action>         Generate or validate tests (new/validate/matrix)
  watch [pattern]           Continuous testing mode
  benchmark [iterations]    Performance benchmark
  build                     Build compiler only
  help                      Show this help

Categories for filter:
  basics, advanced, control, functions, memory, struct

Examples:
  $0 quick                           # Quick test run
  $0 quick tests/basics/arithmetic.dr # Test specific file
  $0 run "basics/**/*.dr"           # Run basic tests
  $0 run "**/*.dr" true             # Run all tests with debug
  $0 bulk                           # Test all categories
  $0 fast                           # Fast basic test
  $0 filter basics                  # Test basic category
  $0 analyze html report.html       # Generate HTML report
  $0 list "advanced/**/*.dr"        # List advanced tests
  $0 generate validate              # Validate existing tests
  $0 watch "basics/**/*.dr"         # Watch mode for basics
  $0 benchmark 5                    # Run 5 benchmark iterations

Environment Variables:
  TEST_DEBUG=1                      # Enable debug output
  TEST_TIMEOUT=30                   # Set test timeout in seconds
EOF
}

# Main command dispatcher
main() {
    # Check dependencies first
    check_dependencies
    
    local command="$1"
    shift || true
    
    case "$command" in
        "quick")
            quick_test "$1"
            ;;
        "run")
            run_tests "$1" "$2" "$3"
            ;;
        "bulk")
            bulk_test
            ;;
        "fast")
            fast_test "$1"
            ;;
        "filter")
            if [ -z "$1" ]; then
                log_error "Category required for filter command"
                exit 1
            fi
            local pattern
            pattern=$(filter_tests "$1")
            run_tests "$pattern"
            ;;
        "analyze")
            analyze_results "$1" "$2"
            ;;
        "list")
            list_tests "$1"
            ;;
        "status")
            show_status
            ;;
        "generate")
            generate_tests "$1"
            ;;
        "watch")
            watch_tests "$1"
            ;;
        "benchmark")
            benchmark_tests "$1"
            ;;
        "build")
            build_compiler
            ;;
        "help"|"--help"|"-h"|"")
            show_help
            ;;
        *)
            log_error "Unknown command: $command"
            echo
            show_help
            exit 1
            ;;
    esac
}

# Execute main function with all arguments
main "$@"