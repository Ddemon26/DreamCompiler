# Getting Started with Dream

Welcome to Dream! This section will help you get up and running with the Dream programming language quickly and easily.

## Contents

### [Quick Start](intro.md)
Get Dream installed and write your first program:
- **Installation** - Download and set up the Dream compiler
- **Your First Program** - Write, compile, and run "Hello World"
- **Basic Concepts** - Variables, functions, and console output
- **Next Steps** - Where to go after your first program

### [Installation & Usage](usage.md)  
Detailed setup and configuration instructions:
- **System Requirements** - Supported operating systems and prerequisites
- **Installation Options** - Package managers, source builds, and binaries
- **Configuration** - Environment variables and compiler settings
- **IDE Integration** - Setting up your development environment
- **Troubleshooting** - Common installation and setup issues

### [Examples & Tutorials](examples/)
Hands-on learning with practical code examples:
- **Basic Examples** - Simple programs demonstrating core concepts
- **Step-by-step Tutorials** - Guided learning exercises
- **Common Patterns** - Frequently used programming patterns
- **Real-world Projects** - Complete application examples

## Quick Installation

### Windows
```bash
# Using the installer (recommended)
# Download DreamCompiler-Setup.exe from releases
# Run installer and follow prompts

# Using Chocolatey
choco install dreamcompiler

# Verify installation
dream --version
```

### Linux  
```bash
# Using package manager (Ubuntu/Debian)
sudo apt update
sudo apt install dreamcompiler

# Using Snap
sudo snap install dreamcompiler

# From source
git clone https://github.com/company/DreamCompiler.git
cd DreamCompiler
zig build
```

### macOS
```bash
# Using Homebrew
brew install dreamcompiler

# Using MacPorts
sudo port install dreamcompiler

# Verify installation
dream --version
```

## Your First Dream Program

### 1. Create a new file called `hello.dr`:
```dream
func main() {
    Console.WriteLine("Hello, World!");
}
```

### 2. Compile and run:
```bash
dream hello.dr
./hello         # Linux/macOS  
hello.exe       # Windows
```

### 3. Expected output:
```
Hello, World!
```

## Quick Examples

### Interactive Input
```dream
func main() {
    Console.Write("What's your name? ");
    string name = Console.ReadLine();
    Console.WriteLine("Hello, " + name + "!");
}
```

### Simple Calculations
```dream
func main() {
    int a = 10;
    int b = 5;
    
    Console.WriteLine("Addition: " + (a + b));
    Console.WriteLine("Multiplication: " + (a * b));
    Console.WriteLine("Division: " + (a / b));
}
```

### Using Functions
```dream
func int add(int x, int y) {
    return x + y;
}

func main() {
    int result = add(15, 25);
    Console.WriteLine("15 + 25 = " + result);
}
```

### Working with Arrays
```dream
func main() {
    int[] numbers = [1, 2, 3, 4, 5];
    
    Console.WriteLine("Numbers:");
    for (int i = 0; i < numbers.Length; i++) {
        Console.WriteLine("  " + numbers[i]);
    }
}
```

## Learning Path

### Beginner (First Hour)
1. **[Quick Start](intro.md)** - Install Dream and run first program
2. **Basic syntax** - Variables, functions, console I/O
3. **Simple calculations** - Arithmetic and string operations
4. **Control flow** - If statements and loops

### Beginner+ (First Day)
5. **[Fundamentals](../fundamentals/)** - Type system and variables
6. **[Language Features](../language-features/)** - Functions and operators
7. **Arrays and collections** - Working with data structures
8. **File operations** - Reading and writing files

### Intermediate (First Week)
9. **[Advanced Topics](../advanced-topics/)** - Memory management and modules
10. **Error handling** - Exceptions and defensive programming
11. **Object-oriented programming** - Classes and inheritance
12. **Project organization** - Multi-file applications

### Advanced (Ongoing)
13. **[C Interoperability](../advanced-topics/interop.md)** - System integration
14. **[API Documentation](../api/)** - Master the standard library
15. **[Development](../development/)** - Contributing to Dream
16. **Performance optimization** - Writing efficient code

## Development Environment Setup

### Visual Studio Code (Recommended)
```json
// Install Dream extension from marketplace
// Add to settings.json:
{
    "dream.compilerPath": "/usr/local/bin/dream",
    "dream.enableIntelliSense": true,
    "dream.formatOnSave": true
}
```

### Other Editors
- **Vim/Neovim** - Dream syntax highlighting plugin available
- **Emacs** - Dream mode for syntax highlighting and compilation
- **IntelliJ/CLion** - Plugin for Dream development
- **Sublime Text** - Syntax highlighting package

## Common First Steps

### 1. Verify Installation
```bash
dream --version     # Check Dream compiler version
dream --help        # View available options
```

### 2. Create Project Directory
```bash
mkdir my-dream-project
cd my-dream-project
```

### 3. Write Test Program
```dream
// main.dr
func main() {
    Console.WriteLine("Dream is working!");
    
    // Test basic functionality
    string name = "Developer";
    int year = 2024;
    
    Console.WriteLine($"Hello {name}, welcome to {year}!");
}
```

### 4. Compile and Test
```bash
dream main.dr
./main          # Should output greeting messages
```

### 5. Explore Examples
```bash
# Download example projects
git clone https://github.com/company/dream-examples.git
cd dream-examples
```

## Troubleshooting

### Common Issues

#### "dream: command not found"
- **Solution**: Ensure Dream is installed and in your PATH
- **Windows**: Add installation directory to PATH environment variable
- **Linux/macOS**: May need to restart terminal or source ~/.bashrc

#### Compilation Errors
```bash
# Enable verbose compilation to see detailed errors
dream --verbose main.dr

# Check syntax with dry-run
dream --check main.dr
```

#### Runtime Errors
```bash
# Run with debug information
dream --debug main.dr

# Enable memory debugging (debug builds)
export DREAM_DEBUG_MEMORY=1
./main
```

#### Performance Issues
```bash
# Compile with optimizations
dream --optimize main.dr

# Profile compilation time
time dream main.dr
```

### Getting Help

#### Documentation
- **[Language Reference](../reference/language-reference.md)** - Complete language guide
- **[API Documentation](../api/)** - Built-in functions and classes
- **[Developer Guide](../development/)** - Advanced topics and contribution

#### Community Support  
- **GitHub Issues** - Bug reports and feature requests
- **Discussion Forums** - Community questions and answers
- **Stack Overflow** - Tag your questions with `dream-lang`

#### Professional Support
- **Commercial Support** - Available for enterprise users
- **Training** - Workshops and certification programs
- **Consulting** - Migration and optimization services

## What's Next?

### Immediate Next Steps
1. **Complete the [Quick Start](intro.md)** tutorial
2. **Read [Installation & Usage](usage.md)** for detailed setup  
3. **Try the [Examples](examples/)** to see Dream in action
4. **Move to [Fundamentals](../fundamentals/)** to learn core concepts

### Building Your First Real Project
1. **Choose a project** - Calculator, file organizer, or simple game
2. **Plan the structure** - Break down into functions and modules
3. **Implement incrementally** - Start simple and add features
4. **Test thoroughly** - Use Dream's testing capabilities
5. **Share your work** - Contribute back to the community

Welcome to the Dream programming language community! We're excited to see what you'll build.