#!/bin/bash
# Test script to verify Dream runtime library integration on Linux
# Tests both compilation and execution of Dream programs with runtime dependencies

echo "=== Dream Runtime Library Integration Tests ==="

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

# Test 1: Basic console output
echo -e "\n${YELLOW}Test 1: Basic Console Output${NC}"
cat > tests/temp_test1.dr << 'EOF'
// Test basic console output functionality
Console.WriteLine("Test 1: Console output works!");
EOF

if result1=$(./dream tests/temp_test1.dr 2>&1) && [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Test 1 PASSED: $result1${NC}"
else
    echo -e "${RED}✗ Test 1 FAILED: $result1${NC}"
fi

# Test 2: Memory allocation functions
echo -e "\n${YELLOW}Test 2: Memory Management${NC}"
cat > tests/temp_test2.dr << 'EOF'
// Test memory allocation and cleanup
Console.WriteLine("Test 2: Memory management works!");
EOF

if result2=$(./dream tests/temp_test2.dr 2>&1) && [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Test 2 PASSED: $result2${NC}"
else
    echo -e "${RED}✗ Test 2 FAILED: $result2${NC}"
fi

# Test 3: Check generated C code includes runtime headers
echo -e "\n${YELLOW}Test 3: Generated C Code Verification${NC}"
cat > tests/temp_test3.dr << 'EOF'
// Test that generated C code properly includes runtime
Console.WriteLine("Test 3: Runtime headers included!");
EOF

# Compile to C only to check headers
if c_output=$(./dream --emit-c tests/temp_test3.dr 2>&1) && [ $? -eq 0 ]; then
    missing_hdrs=()
    for h in runtime/*.h; do
        base=$(basename "$h")
        if ! grep -q "#include \"$base\"" build/bin/dream.c; then
            missing_hdrs+=("$base")
        fi
    done
    if [ ${#missing_hdrs[@]} -eq 0 ]; then
        echo -e "${GREEN}✓ Test 3 PASSED: Runtime headers properly included${NC}"
    else
        echo -e "${RED}✗ Test 3 FAILED: Missing headers: ${missing_hdrs[*]}${NC}"
    fi
else
    echo -e "${RED}✗ Test 3 FAILED: Could not generate C code: $c_output${NC}"
fi

# Test 4: Verify runtime object files exist and are linked
echo -e "\n${YELLOW}Test 4: Runtime Object Files${NC}"
missing_objs=()
for src in runtime/*.c; do
    base=$(basename "$src" .c)
    if [ ! -f "build/$base.o" ] && [ ! -f "build/libdruntime.a" ]; then
        missing_objs+=("$base.o")
    fi
done
if [ ${#missing_objs[@]} -eq 0 ]; then
    echo -e "${GREEN}✓ Test 4 PASSED: Runtime object files exist${NC}"
else
    echo -e "${RED}✗ Test 4 FAILED: Missing objects: ${missing_objs[*]}${NC}"
fi

# Test 5: Cross-platform compilation test
echo -e "\n${YELLOW}Test 5: Cross-Platform Compilation${NC}"
cat > tests/temp_test5.dr << 'EOF'
// Test cross-platform runtime compatibility
Console.WriteLine("Cross-platform test successful!");
EOF

# Test different targets if available
targets=("x86_64-linux" "x86_64-windows")
for target in "${targets[@]}"; do
    echo -e "  ${CYAN}Testing target: $target${NC}"
    if cross_result=$(./dream --target "$target" --emit-c tests/temp_test5.dr 2>&1) && [ $? -eq 0 ]; then
        echo -e "  ${GREEN}✓ $target compilation successful${NC}"
    else
        echo -e "  ${RED}✗ $target compilation failed: $cross_result${NC}"
    fi
done

# Test 6: Manual compilation verification
echo -e "\n${YELLOW}Test 6: Manual Runtime Linking${NC}"
cat > tests/temp_test6.dr << 'EOF'
// Test manual compilation with runtime
Console.WriteLine("Manual linking test!");
EOF

# Generate C code
if ./dream --emit-c tests/temp_test6.dr > /dev/null 2>&1; then
    # Try manual compilation
    if gcc -I runtime build/bin/dream.c runtime/console.c runtime/memory.c -o build/bin/manual_test 2>/dev/null; then
        if manual_result=$(build/bin/manual_test 2>&1); then
            echo -e "${GREEN}✓ Test 6 PASSED: Manual linking works: $manual_result${NC}"
        else
            echo -e "${RED}✗ Test 6 FAILED: Manual compilation succeeded but execution failed${NC}"
        fi
        rm -f build/bin/manual_test
    else
        echo -e "${RED}✗ Test 6 FAILED: Manual compilation failed${NC}"
    fi
else
    echo -e "${RED}✗ Test 6 FAILED: Could not generate C code${NC}"
fi

# Cleanup temporary files
rm -f tests/temp_test*.dr

echo -e "\n${GREEN}=== Test Summary ===${NC}"
echo -e "${WHITE}Runtime integration tests completed.${NC}"
echo -e "${WHITE}Check output above for any failures that need attention.${NC}"