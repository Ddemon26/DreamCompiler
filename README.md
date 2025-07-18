# Dream Compiler

![Dream Compiler Logo](assets/icons/DreamCompilerLogo.png)

Dream is an advanced compiler that transforms a C#-like language called Dream into C code. The project is experimental
but aims to grow into a fully featured compiler. This repository contains the source, example code, tests and documentation.

## Features

The compiler currently supports:
- Primitive types and variable declarations (with optional initialisers)
- Arrays of primitive types
- Arithmetic, bitwise and comparison operators
- Control flow statements: `if`/`else`, loops and `switch`
- Functions with parameters and return values
- Increment/decrement and compound assignment operators
- Ternary operator `?:` and string concatenation with `+`
- Console input/output via `Console.ReadLine`, `Console.Write` and `Console.WriteLine`
See the [changelog](docs/v1.1/changelog.md) for details on recent additions.

## Getting Started

The steps below show how to build and use Dream on both Linux and Windows.

1. **Install prerequisites**
   - Linux: `sudo apt update && sudo apt install -y build-essential gcc`
   - Download Zig 0.15.0: [Linux x86_64 build](https://ziglang.org/builds/zig-x86_64-linux-0.15.0-dev.936+fc2c1883b.tar.xz) or [Windows aarch64 build](https://ziglang.org/builds/zig-aarch64-windows-0.15.0-dev.936+fc2c1883b.zip). Extract the archive and add the `zig` binary to your `PATH`.
   - Windows also requires [Git for Windows](https://git-scm.com/).
2. **Clone the repository**
   ```bash
   git clone https://github.com/Ddemon26/DreamCompiler.git
   cd DreamCompiler
   ```
3. **Build the compiler**
   ```bash
   zig build
   ```
4. **Run the example**
   ```bash
   zig build run -- example.dr
   ```
5. **Run the tests**
   ```bash
   zig build test
   ```

The resulting compiler binary is placed under `zig-out/bin`.

## Using the Compiler

Compile a `.dr` file directly:

```bash
./zig-out/bin/DreamCompiler path/to/file.dr
```

This writes `build/bin/dream.c` and builds a runnable program called `dream` in the current directory. Execute it with:

```bash
./dream
```

An example program is provided in [example.dr](example.dr).

## Documentation

Documentation lives under [`docs`](docs). Open [docs/index.html](docs/index.html) in a browser or read [docs/index.md](docs/index.md) directly.

See [Compiler Docs](docs/compiler/index.md) for compiler information and [Dream.dr Language](docs/language/index.md) for language guides.
## Contributing

Contributions are welcome. Document any new language feature under `docs/` with a matching test case in `tests/`. Add additional dependencies to [`codex/_startup.sh`](codex/_startup.sh) if required.

## License

Dream Compiler is released under the [MIT License](LICENSE).

## Syntax Highlighting

Generate the VS Code grammar from `src/lexer/tokens.def`. The generator
also syncs copies under `assets/jetbrains` used by the JetBrains plugin:

```bash
node assets/jetbrains/scripts/genFromTokens.js
```

### VS Code

```bash
cd assets/vscode
npm install
npx vsce package
```

### JetBrains Plugin

```bash
cd assets/jetbrains
./gradlew generateLexer build test
```
Ensure a JDK 17 is available. The build regenerates the lexer from `tokens.def` before compiling and running the tests.

The resulting VSIX and plugin zip live in their respective directories.

## Custom file icon

Both editors show a Dream logo for `.dr` files after installing the extensions.
