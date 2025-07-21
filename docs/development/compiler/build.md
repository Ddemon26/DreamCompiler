# Build System Documentation

The Dream Compiler uses Zig as its build system, providing cross-platform compilation, fast builds, and flexible configuration. This document covers the complete build system architecture and usage.

## Overview

The build system compiles C source files using Zig's cross-compilation capabilities while providing Dream-specific optimizations and configurations.

```
+------------------------------------------------------------+
|                    Dream Build Pipeline                    |
+------------------------------------------------------------+
|  build.zig (Configuration)                                 |
|              |                                             |
|              v                                             |
|  Zig Build System                                          |
|              |                                             |
|              v                                             |
|  C Compiler (zig cc)                                       |
|              |                                             |
|              v                                             |
|  Linker (zig ld)                                           |
|              |                                             |
|              v                                             |
|  Native Executable                                         |
+------------------------------------------------------------+
```

## Build Configuration (`build.zig`)

The main build configuration file defines all source files, dependencies, and compilation options:

```zig
const std = @import("std");

pub fn build(b: *std.Build) void {
    // Target and optimization configuration
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    
    // Main compiler executable
    const exe = b.addExecutable(.{
        .name = "DreamCompiler",
        .target = target,
        .optimize = optimize,
    });
    
    // Add all C source files
    addCompilerSources(exe);
    addRuntimeSources(exe);
    
    // Configure compilation options
    configureCompilation(exe);
    
    // Install artifacts
    b.installArtifact(exe);
    
    // Define build commands
    defineBuildCommands(b, exe);
}
```

### Source File Organization

All source files are organized in the `AllCSources` array:

```zig
const AllCSources = [_][]const u8{
    // Core compiler files
    "src/main.c",
    "src/driver/driver.c",
    "src/driver/cli.c",
    
    // Lexical analysis
    "src/lexer/lexer.c",        // Generated from lexer.re
    "src/lexer/token.c",
    
    // Parsing
    "src/parser/parser.c",
    "src/parser/ast.c",
    "src/parser/precedence.c",
    
    // Semantic analysis
    "src/sem/semantic.c",
    "src/sem/symbol_table.c",
    "src/sem/type_checker.c",
    
    // Intermediate representation
    "src/ir/ir.c",
    "src/ir/ir_builder.c",
    "src/ssa/ssa.c",
    "src/cfg/cfg.c",
    
    // Optimization passes
    "src/opt/optimizer.c",
    "src/opt/constant_folding.c",
    "src/opt/dead_code_elimination.c",
    "src/opt/sccp.c",           // Sparse Conditional Constant Propagation
    "src/opt/licm.c",           // Loop Invariant Code Motion
    "src/opt/cse.c",            // Common Subexpression Elimination
    
    // Code generation
    "src/codegen/codegen.c",
    "src/codegen/c_emitter.c",
    "src/codegen/module.c",
    "src/codegen/backend.c",
    
    // Utilities
    "src/utils/memory.c",
    "src/utils/string_utils.c",
    "src/utils/file_utils.c",
    "src/utils/error.c",
    "src/utils/debug.c",
    
    // Runtime library
    "src/runtime/memory.c",
    "src/runtime/console.c",
    "src/runtime/custom.c",
};

fn addCompilerSources(exe: *std.Build.Step.Compile) void {
    for (AllCSources) |source| {
        exe.addCSourceFile(.{
            .file = .{ .path = source },
            .flags = &[_][]const u8{
                "-std=c11",
                "-Wall",
                "-Wextra",
                "-Wpedantic",
                "-O2",
            },
        });
    }
}
```

### Runtime Library Integration

The runtime library is built as part of the main executable:

```zig
fn addRuntimeSources(exe: *std.Build.Step.Compile) void {
    const runtime_sources = [_][]const u8{
        "src/runtime/memory.c",
        "src/runtime/console.c", 
        "src/runtime/custom.c",
    };
    
    for (runtime_sources) |source| {
        exe.addCSourceFile(.{
            .file = .{ .path = source },
            .flags = &[_][]const u8{
                "-std=c11",
                "-DDREAM_RUNTIME",
                "-O2",
            },
        });
    }
}
```

## Build Commands

### Basic Build Commands

```bash
# Build the compiler
zig build

# Build with specific optimization
zig build -Doptimize=ReleaseFast
zig build -Doptimize=ReleaseSafe
zig build -Doptimize=ReleaseSmall
zig build -Doptimize=Debug

# Build for specific target
zig build -Dtarget=x86_64-linux-gnu
zig build -Dtarget=x86_64-windows-gnu
zig build -Dtarget=aarch64-macos
```

### Advanced Build Options

```bash
# Enable debug information
zig build -Ddebug=true

# Enable memory debugging
zig build -Ddebug-memory=true

# Enable profile-guided optimization
zig build -Dpgo=true

# Build with custom flags
zig build -Dextra-flags="-DCUSTOM_FLAG=1"

# Static linking
zig build -Dstatic=true

# Cross-compilation with system libraries
zig build -Dtarget=x86_64-linux-gnu -Dlink-libc
```

### Development Build Commands

```bash
# Development build with all debugging enabled
zig build dev

# Quick build for testing (minimal optimization)
zig build quick

# Clean build (remove all artifacts)
zig build clean

# Install to system path
zig build install --prefix /usr/local
```

## Build Targets and Configurations

### Optimization Modes

```zig
fn configureOptimization(exe: *std.Build.Step.Compile, optimize: std.builtin.OptimizeMode) void {
    switch (optimize) {
        .Debug => {
            exe.defineCMacro("DEBUG", "1");
            exe.defineCMacro("DREAM_DEBUG_MEMORY", null);
            exe.defineCMacro("DREAM_DEBUG_PARSING", null);
        },
        .ReleaseFast => {
            exe.defineCMacro("NDEBUG", "1");
            exe.defineCMacro("DREAM_OPTIMIZE_SPEED", null);
        },
        .ReleaseSmall => {
            exe.defineCMacro("NDEBUG", "1");
            exe.defineCMacro("DREAM_OPTIMIZE_SIZE", null);
        },
        .ReleaseSafe => {
            exe.defineCMacro("DREAM_SAFE_MODE", "1");
        },
    }
}
```

### Platform-Specific Configuration

```zig
fn configurePlatform(exe: *std.Build.Step.Compile, target: std.zig.CrossTarget) void {
    switch (target.getOsTag()) {
        .windows => {
            exe.defineCMacro("_WIN32", "1");
            exe.defineCMacro("DREAM_WINDOWS", "1");
            exe.linkSystemLibrary("kernel32");
            exe.linkSystemLibrary("user32");
        },
        .linux => {
            exe.defineCMacro("__linux__", "1");
            exe.defineCMacro("DREAM_LINUX", "1");
            exe.linkSystemLibrary("pthread");
            exe.linkSystemLibrary("m");
        },
        .macos => {
            exe.defineCMacro("__APPLE__", "1");
            exe.defineCMacro("DREAM_MACOS", "1");
            exe.linkFramework("Foundation");
        },
        else => {},
    }
}
```

## Multi-File Compilation Support

The build system includes support for multi-file Dream projects:

```zig
fn defineMultiFileCommands(b: *std.Build, exe: *std.Build.Step.Compile) void {
    const compile_multi = b.step("compile-multi", "Compile multi-file Dream project");
    
    const multi_cmd = b.addRunArtifact(exe);
    multi_cmd.addArgs(&[_][]const u8{
        "--multi-file",
        "--project-root", ".",
        "--output", "build/bin/dream"
    });
    
    compile_multi.dependOn(&multi_cmd.step);
}
```

### Project Structure Support

```zig
// Project configuration for multi-file compilation
const ProjectConfig = struct {
    name: []const u8,
    root_module: []const u8,
    source_dirs: []const []const u8,
    dependencies: []const []const u8,
    output_dir: []const u8,
};

fn buildProject(b: *std.Build, exe: *std.Build.Step.Compile, config: ProjectConfig) void {
    const project_step = b.step(config.name, "Build Dream project");
    
    const build_cmd = b.addRunArtifact(exe);
    build_cmd.addArgs(&[_][]const u8{
        "--project", config.name,
        "--root", config.root_module,
        "--output", config.output_dir,
    });
    
    for (config.source_dirs) |dir| {
        build_cmd.addArg("--source-dir");
        build_cmd.addArg(dir);
    }
    
    project_step.dependOn(&build_cmd.step);
}
```

## Runtime Library Building

### Separate Runtime Library

The runtime can also be built as a separate static library:

```zig
fn buildRuntimeLibrary(b: *std.Build) *std.Build.Step.Compile {
    const lib = b.addStaticLibrary(.{
        .name = "dreamrt",
        .target = target,
        .optimize = optimize,
    });
    
    // Add runtime source files
    const runtime_files = [_][]const u8{
        "src/runtime/memory.c",
        "src/runtime/console.c",
        "src/runtime/custom.c",
    };
    
    for (runtime_files) |file| {
        lib.addCSourceFile(.{
            .file = .{ .path = file },
            .flags = &[_][]const u8{
                "-std=c11",
                "-DDREAM_RUNTIME_LIB",
                "-O2",
            },
        });
    }
    
    // Install library
    b.installArtifact(lib);
    return lib;
}
```

### Runtime Header Generation

Generate headers for C interop:

```zig
fn generateRuntimeHeaders(b: *std.Build) void {
    const gen_headers = b.step("gen-headers", "Generate runtime headers");
    
    const header_cmd = b.addSystemCommand(&[_][]const u8{
        "python3", "scripts/generate_headers.py",
        "--input", "src/runtime/",
        "--output", "include/dream/",
    });
    
    gen_headers.dependOn(&header_cmd.step);
}
```

## Testing Integration

### Unit Tests

```zig
fn addUnitTests(b: *std.Build) void {
    const test_exe = b.addTest(.{
        .name = "dream-tests",
        .root_source_file = .{ .path = "tests/unit/main.zig" },
    });
    
    // Add C sources for testing
    for (AllCSources) |source| {
        test_exe.addCSourceFile(.{
            .file = .{ .path = source },
            .flags = &[_][]const u8{"-std=c11", "-DUNIT_TESTING"},
        });
    }
    
    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&b.addRunArtifact(test_exe).step);
}
```

### Integration Tests

```zig
fn addIntegrationTests(b: *std.Build, exe: *std.Build.Step.Compile) void {
    const test_step = b.step("test-integration", "Run integration tests");
    
    // Test basic compilation
    const basic_test = b.addRunArtifact(exe);
    basic_test.addArgs(&[_][]const u8{
        "tests/basics/hello.dr",
        "--output", "/tmp/hello_test"
    });
    
    // Test multi-file compilation
    const multi_test = b.addRunArtifact(exe);
    multi_test.addArgs(&[_][]const u8{
        "--multi-file",
        "tests/multifile/main.dr",
        "--output", "/tmp/multifile_test"
    });
    
    test_step.dependOn(&basic_test.step);
    test_step.dependOn(&multi_test.step);
}
```

## Custom Build Steps

### Lexer Generation

The lexer is generated from a re2c specification:

```zig
fn generateLexer(b: *std.Build) void {
    const gen_lexer = b.step("gen-lexer", "Generate lexer from re2c");
    
    const lexer_cmd = b.addSystemCommand(&[_][]const u8{
        "re2c",
        "-c",
        "-o", "src/lexer/lexer.c",
        "src/lexer/lexer.re"
    });
    
    gen_lexer.dependOn(&lexer_cmd.step);
}
```

### Documentation Generation

```zig
fn generateDocs(b: *std.Build) void {
    const gen_docs = b.step("docs", "Generate documentation");
    
    const docs_cmd = b.addSystemCommand(&[_][]const u8{
        "python3", "scripts/generate_docs.py",
        "--source", "docs/",
        "--output", "build/docs/",
        "--format", "html"
    });
    
    gen_docs.dependOn(&docs_cmd.step);
}
```

## Build Performance Optimization

### Parallel Compilation

```zig
fn configureParallelBuild(exe: *std.Build.Step.Compile) void {
    // Enable parallel compilation
    exe.setMaxRssBytes(4 * 1024 * 1024 * 1024); // 4GB max RSS
    
    // Use all available CPU cores
    exe.addCSourceFiles(&AllCSources, &[_][]const u8{
        "-std=c11",
        "-Wall",
        "-O2",
        "-j", "0", // Use all cores
    });
}
```

### Incremental Builds

```zig
fn configureIncrementalBuild(b: *std.Build, exe: *std.Build.Step.Compile) void {
    // Enable incremental compilation
    exe.defineCMacro("DREAM_INCREMENTAL", "1");
    
    // Cache build artifacts
    const cache_dir = b.cache_root.join(b.allocator, &[_][]const u8{"dream"}) catch unreachable;
    exe.addIncludePath(.{ .path = cache_dir });
}
```

## Debugging and Profiling Builds

### Debug Configuration

```zig
fn configureDebugBuild(exe: *std.Build.Step.Compile) void {
    // Add debug symbols
    exe.addCSourceFiles(&AllCSources, &[_][]const u8{
        "-std=c11",
        "-g",
        "-O0",
        "-DDEBUG",
        "-DDREAM_DEBUG_MEMORY",
        "-DDREAM_DEBUG_PARSING",
        "-fsanitize=address",
        "-fsanitize=undefined",
    });
    
    // Link with debug libraries
    exe.linkSystemLibrary("asan");
    exe.linkSystemLibrary("ubsan");
}
```

### Profiling Configuration

```zig
fn configureProfilingBuild(exe: *std.Build.Step.Compile) void {
    exe.addCSourceFiles(&AllCSources, &[_][]const u8{
        "-std=c11",
        "-O2",
        "-g",
        "-pg",  // Enable profiling
        "-DDREAM_PROFILING",
    });
}
```

## Build Scripts and Automation

### Cross-Platform Build Scripts

**Linux/macOS (`build.sh`):**
```bash
#!/bin/bash
set -e

echo "Building Dream Compiler..."
zig build --summary all

echo "Running tests..."
zig build test

echo "Installing..."
zig build install --prefix /usr/local

echo "Build complete!"
```

**Windows (`build.bat`):**
```batch
@echo off
echo Building Dream Compiler...
zig build --summary all

echo Running tests...
zig build test

echo Build complete!
```

### CI/CD Integration

**GitHub Actions (`.github/workflows/build.yml`):**
```yaml
name: Build Dream Compiler

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        optimize: [Debug, ReleaseFast]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup Zig
      uses: goto-bus-stop/setup-zig@v2
      with:
        version: 0.12.0
    
    - name: Build
      run: zig build -Doptimize=${{ matrix.optimize }}
    
    - name: Test
      run: zig build test
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v3
      with:
        name: dream-compiler-${{ matrix.os }}-${{ matrix.optimize }}
        path: zig-out/bin/
```

## Build Output and Artifacts

### Standard Output Structure

```
build/
|-- bin/
|   |-- DreamCompiler      # Main executable
|   |-- dream.c            # Generated C code
|   |-- dream              # Final executable
|   '-- libdreamrt.a       # Runtime library
|-- docs/
|   |-- html/              # Generated documentation
|   '-- api/               # API reference
|-- include/
|   |-- dream/             # Public headers
|   '-- runtime/           # Runtime headers
'-- tests/
    |-- results/           # Test results
    '-- coverage/          # Code coverage reports
```

### Installation Paths

```zig
fn configureInstallation(b: *std.Build, exe: *std.Build.Step.Compile) void {
    // Install executable
    b.installArtifact(exe);
    
    // Install headers
    b.installDirectory(.{
        .source_dir = .{ .path = "src/runtime/" },
        .install_dir = .header,
        .install_subdir = "dream",
        .include_extensions = &[_][]const u8{".h"},
    });
    
    // Install documentation
    b.installDirectory(.{
        .source_dir = .{ .path = "docs/" },
        .install_dir = .{ .custom = "share/dream/docs" },
    });
}
```

## Build System Extension

### Custom Build Options

```zig
pub fn build(b: *std.Build) void {
    // Custom build options
    const enable_jit = b.option(bool, "jit", "Enable JIT compilation") orelse false;
    const enable_llvm = b.option(bool, "llvm", "Use LLVM backend") orelse false;
    const max_memory = b.option(u64, "max-memory", "Maximum memory usage in MB") orelse 1024;
    
    const exe = b.addExecutable(.{
        .name = "DreamCompiler",
        .target = target,
        .optimize = optimize,
    });
    
    if (enable_jit) {
        exe.defineCMacro("DREAM_JIT_ENABLED", "1");
        exe.linkSystemLibrary("llvm");
    }
    
    if (enable_llvm) {
        exe.defineCMacro("DREAM_LLVM_BACKEND", "1");
        exe.linkSystemLibrary("LLVM");
    }
    
    exe.defineCMacro("DREAM_MAX_MEMORY", b.fmt("{d}", .{max_memory}));
}
```

### Plugin System

```zig
fn addPluginSupport(b: *std.Build, exe: *std.Build.Step.Compile) void {
    // Enable plugin system
    exe.defineCMacro("DREAM_PLUGINS_ENABLED", "1");
    
    // Add plugin sources
    const plugin_files = [_][]const u8{
        "src/plugins/plugin_system.c",
        "src/plugins/plugin_loader.c",
        "src/plugins/builtin_plugins.c",
    };
    
    for (plugin_files) |file| {
        exe.addCSourceFile(.{
            .file = .{ .path = file },
            .flags = &[_][]const u8{"-std=c11", "-fPIC"},
        });
    }
    
    // Enable dynamic linking for plugins
    exe.linkLibC();
    exe.linkSystemLibrary("dl");
}
```

This comprehensive build system documentation covers all aspects of building, configuring, and extending the Dream Compiler build process.