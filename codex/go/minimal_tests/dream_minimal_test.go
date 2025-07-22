package main

/*
#cgo CFLAGS: -std=c11 -Wall
#include "dream_minimal.h"
*/
import "C"
import (
	"testing"
	"unsafe"
)

// Test basic arithmetic function
func TestArithmetic(t *testing.T) {
	result := C.dream_test_add(5, 3)
	expected := C.int(8)
	
	if result != expected {
		t.Errorf("Expected %d, got %d", expected, result)
	}
}

// Test string length function
func TestStringLength(t *testing.T) {
	testStr := C.CString("Hello, World!")
	defer C.free(unsafe.Pointer(testStr))
	
	result := C.dream_test_string_length(testStr)
	expected := C.int(13)
	
	if result != expected {
		t.Errorf("Expected %d, got %d", expected, result)
	}
	
	// Test null string
	nullResult := C.dream_test_string_length(nil)
	if nullResult != -1 {
		t.Errorf("Expected -1 for null string, got %d", nullResult)
	}
}

// Test compilation check
func TestCompilation(t *testing.T) {
	tests := []struct {
		name     string
		source   string
		expected bool
	}{
		{"Valid Console.WriteLine", `Console.WriteLine("Hello");`, true},
		{"Valid int declaration", `int x = 42;`, true},
		{"Valid string declaration", `string name = "test";`, true},
		{"Empty source", "", false},
		{"Invalid source", "random text", false},
	}
	
	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			cSource := C.CString(test.source)
			defer C.free(unsafe.Pointer(cSource))
			
			result := bool(C.dream_test_compilation(cSource))
			if result != test.expected {
				t.Errorf("For source '%s': expected %v, got %v", test.source, test.expected, result)
			}
		})
	}
}

// Test version function
func TestVersion(t *testing.T) {
	version := C.GoString(C.dream_get_version())
	
	if len(version) == 0 {
		t.Error("Version string should not be empty")
	}
	
	if version != "DreamCompiler v1.0.0" {
		t.Errorf("Unexpected version string: %s", version)
	}
}

// Benchmark arithmetic function
func BenchmarkArithmetic(b *testing.B) {
	for i := 0; i < b.N; i++ {
		C.dream_test_add(C.int(i), C.int(i+1))
	}
}

// Benchmark string length function
func BenchmarkStringLength(b *testing.B) {
	testStr := C.CString("Hello, World!")
	defer C.free(unsafe.Pointer(testStr))
	
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		C.dream_test_string_length(testStr)
	}
}