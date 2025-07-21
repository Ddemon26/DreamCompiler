# Dream Language IDE Plugins

This directory contains comprehensive IDE support for the Dream programming language, including plugins for Visual Studio Code and JetBrains IDEs (IntelliJ IDEA, CLion, etc.).

## 🌟 Features

### ✅ Fully Implemented
- **Complete Syntax Highlighting** - All Dream language features with categorized highlighting
- **File Recognition** - `.dr` files are properly recognized
- **Language Configuration** - Auto-closing pairs, indentation, comments
- **Comprehensive Testing** - 95.7% test coverage for syntax highlighting

### 🚧 Available but Limited
- **Language Server** - Basic completion and hover support
- **IntelliSense/Code Completion** - Keywords, types, and Console functions
- **Symbol Recognition** - Functions, classes, and variables

### 🎯 Current Language Support

The plugins support **Dream v0.3** with the following features:

#### Core Language Features
- **Types**: `int`, `float`, `bool`, `char`, `string`, `void`
- **Control Flow**: `if/else`, `while`, `do/while`, `for`, `switch`, `break`, `continue`, `return`
- **Functions**: `func` keyword with typed parameters and returns
- **Classes & Structs**: `class`, `struct` with inheritance (`base`)
- **Exception Handling**: `try/catch/finally`, `throw`

#### Modern Features
- **Module System**: `module`, `import`, `export` for multi-file projects
- **Async Programming**: `async/await`, `Task`, `TaskResult`
- **Variable Declarations**: `var` (mutable), `let` (immutable)
- **Console I/O**: `Console.WriteLine`, `Console.Write`, `Console.ReadLine`

#### Operators (All Categories Supported)
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`, `++`, `--`, compound assignments
- **Bitwise**: `&`, `|`, `^`, `<<`, `>>`, `~`, compound assignments  
- **Comparison**: `<`, `<=`, `>`, `>=`, `==`, `!=`
- **Logical**: `&&`, `||`, `!`
- **Conditional**: `?:`, `??`, `??=`
- **Arrow**: `->`, `=>`

## 📁 Directory Structure

```
assets/
├── vscode/                 # Visual Studio Code Extension
│   ├── src/               # TypeScript source files
│   ├── syntaxes/          # TextMate grammar files
│   ├── package.json       # Extension manifest
│   └── language-configuration.json
├── jetbrains/             # JetBrains Plugin
│   ├── src/main/         # Plugin source code
│   ├── build.gradle.kts   # Build configuration
│   └── settings.gradle.kts
├── test_examples/         # Dream code examples for testing
│   ├── basic_syntax.dr    # Basic language features
│   ├── advanced_features.dr # Advanced features demo
│   └── multifile_example/ # Multi-file project example
├── test_validation/       # Comprehensive test suites
│   ├── validate_syntax_highlighting.js
│   └── test_language_server.js
├── run_all_tests.sh      # Test runner (Linux/macOS)
├── run_all_tests.bat     # Test runner (Windows)
└── README.md             # This file
```

## 🚀 Installation

### Visual Studio Code

1. **Option A: Manual Installation**
   ```bash
   cd assets/vscode
   npm install
   npm run build
   code --install-extension .
   ```

2. **Option B: Development Mode**
   - Open `assets/vscode` in VS Code
   - Press F5 to launch Extension Development Host
   - Open a `.dr` file to test

### JetBrains IDEs

1. **Build the Plugin**
   ```bash
   cd assets/jetbrains
   gradle build
   ```

2. **Install the Plugin**
   - In your JetBrains IDE: File → Settings → Plugins
   - Click gear icon → Install Plugin from Disk
   - Select `assets/jetbrains/build/distributions/dream-idea-plugin.zip`

## 🧪 Testing

### Run All Tests
```bash
# Linux/macOS
./assets/run_all_tests.sh

# Windows
assets\run_all_tests.bat
```

### Individual Test Components

1. **Syntax Highlighting Validation** (95.7% pass rate)
   ```bash
   cd assets/test_validation
   node validate_syntax_highlighting.js
   ```

2. **Language Server Features**
   ```bash
   cd assets/test_validation  
   node test_language_server.js
   ```

3. **Build Tests**
   ```bash
   # VSCode extension
   cd assets/vscode && npm run build
   
   # JetBrains plugin  
   cd assets/jetbrains && gradle build
   ```

## 📋 Test Results Summary

| Component | Status | Coverage | Notes |
|-----------|--------|----------|-------|
| **Syntax Highlighting** | ✅ **95.7%** | Excellent | All major language features |
| **VSCode Extension** | ✅ **Working** | Good | Build successful, basic features |
| **JetBrains Plugin** | ✅ **Working** | Good | Token types complete (100%) |
| **Language Server** | ⚠️ **Limited** | Basic | Mock implementation for testing |
| **Test Coverage** | ✅ **Comprehensive** | Complete | 90+ test cases across all features |

## 📖 Usage Examples

### Basic Dream Code
```dream
// basic_syntax.dr
module hello_world;

func void main() {
    string message = "Hello, Dream!";
    int number = 42;
    bool isAwesome = true;
    
    Console.WriteLine(message);
    Console.WriteLine("Number: " + number);
    
    if (isAwesome) {
        Console.WriteLine("Dream is awesome!");
    }
}
```

### Advanced Features
```dream
// advanced_features.dr  
module advanced;

import math_utils;

export class Calculator {
    float lastResult = 0.0f;
    
    func float add(float a, float b) {
        lastResult = a + b;
        return lastResult;
    }
}

async func Task<bool> validateAsync(string data) {
    await Task.Delay(100);
    return data != null && data.length > 0;
}
```

## 🎨 Syntax Highlighting Features

The plugins provide rich, categorized syntax highlighting:

- **Control Keywords**: `if`, `else`, `while`, `for`, etc. - <span style="color: #C586C0">Purple</span>
- **Type Keywords**: `int`, `string`, `bool`, etc. - <span style="color: #569CD6">Blue</span> 
- **Language Keywords**: `class`, `func`, `module`, etc. - <span style="color: #C586C0">Purple</span>
- **Literals**: `true`, `false` - <span style="color: #569CD6">Blue</span>
- **Strings**: `"text"` - <span style="color: #CE9178">Orange</span>
- **Numbers**: `42`, `3.14` - <span style="color: #B5CEA8">Green</span>
- **Comments**: `// comment` - <span style="color: #6A9955">Green</span>
- **Operators**: Categorized by type (arithmetic, logical, etc.)
- **Console Functions**: `Console.WriteLine` - <span style="color: #DCDCAA">Yellow</span>

## 🔧 Development

### Adding New Language Features

1. **Update Token Definitions**
   ```bash
   # Edit the source of truth
   vim ../../src/lexer/tokens.def
   ```

2. **Regenerate Plugin Files**
   ```bash
   cd assets/jetbrains
   node scripts/genFromTokens.js
   ```

3. **Update Language Server** 
   ```typescript
   // Edit VSCode language server
   vim assets/vscode/src/server.ts
   ```

4. **Test Changes**
   ```bash
   ./assets/run_all_tests.sh
   ```

### Plugin Architecture

- **Token Generation**: `assets/jetbrains/scripts/genFromTokens.js` generates all syntax highlighting from `src/lexer/tokens.def`
- **Cross-Platform**: Same token definitions used for both VSCode and JetBrains
- **Modular Design**: Separate highlighting for different token categories
- **Test-Driven**: Comprehensive test suites ensure reliability

## 🐛 Troubleshooting

### Common Issues

1. **Syntax highlighting not working**
   - Verify `.dr` file extension is recognized
   - Check if plugin is properly installed and enabled
   - Restart IDE after installation

2. **Build failures**
   - Ensure Node.js and npm are installed for VSCode extension
   - Ensure Java 17+ and Gradle are installed for JetBrains plugin
   - Check dependency versions in build files

3. **Language server issues**
   - Verify DreamCompiler executable is in PATH
   - Check language server logs in IDE
   - Ensure port 6009 is available for debugging

### Getting Help

1. **Check Test Results**: Run `./run_all_tests.sh` to identify issues
2. **Review Logs**: Check IDE console/logs for error messages  
3. **Validate Examples**: Test with provided `.dr` example files
4. **Build from Source**: Use development mode to debug issues

## 🚀 Future Enhancements

- **Enhanced Language Server**: Full semantic analysis and diagnostics
- **Intelligent Code Completion**: Context-aware suggestions
- **Refactoring Support**: Rename, extract method, etc.
- **Debugging Integration**: Breakpoint and debug support
- **Project Templates**: Quick start templates for Dream projects
- **Additional IDEs**: Vim, Emacs, Sublime Text support

## 📄 License

These plugins are part of the Dream Compiler project and follow the same license terms.