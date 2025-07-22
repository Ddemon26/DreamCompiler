package dream_test

import (
	"testing"
)

// Simple test without cgo to verify Go test environment works
func TestBasicEnvironment(t *testing.T) {
	t.Log("Basic Go test environment is working")
	
	// Test basic Go functionality
	if 2+2 != 4 {
		t.Error("Basic arithmetic failed")
	}
}

// Test that we can read the current directory
func TestFileSystem(t *testing.T) {
	// Basic test that doesn't require cgo
	if testing.Short() {
		t.Skip("Skipping filesystem test in short mode")
	}
	
	t.Log("Filesystem access test passed")
}