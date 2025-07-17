# Dream Compiler

Dream is an advanced compiler that transforms a C#-like language called Dream into C code. The project is experimental
but aims to grow into a fully featured compiler. This repository contains the source, example code, tests and documentation.

## Features

The compiler currently supports:

See the [changelog](docs/changelog.md) for details on recent additions.

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

Documentation for language features lives under [`docs`](docs). Start with [index.md](docs/index.md) to browse the available guides.

See [Source Architecture](docs/v1/architecture.md) for an overview of the compiler layout.
## Contributing

Contributions are welcome. Document any new language feature under `docs/` with a matching test case in `tests/`. Add additional dependencies to [`codex/_startup.sh`](codex/_startup.sh) if required.

## License

Dream Compiler is released under the [MIT License](LICENSE).

## Syntax Highlighting

Generate the VS Code grammar from `tokens.json` at the repository root. The generator
also syncs copies under `idea/` used by the JetBrains plugin:

```bash
node scripts/genFromTokens.js
```

### VS Code

```bash
cd vscode
npm install
npx vsce package
```

### JetBrains Plugin

```bash
cd idea
./gradlew generateLexer build test
```
Ensure a JDK 17 is available. The build regenerates the lexer from `tokens.json` before compiling and running the tests.

The resulting VSIX and plugin zip live in their respective directories.

## Custom file icon

Both editors show a Dream logo for `.dr` files after installing the extensions.
