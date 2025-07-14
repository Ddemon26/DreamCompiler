# Dream Compiler

Dream is a minimal compiler that transforms a C#-like language into C code. The project is experimental but aims to grow into a fully featured compiler. This repository contains the source, example code, tests and documentation.

## Features

The compiler currently supports:

- Integer variables and assignments
- Basic arithmetic with `+` and `-`
- Console output via `Console.WriteLine`
- Simple `if` statements
- `while` loops

More features such as functions and string output are planned for future versions. See the [changelog](docs/changelog.md) for details.

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
5. **Run the tests (optional)**
   ```bash
   for f in tests/*.dr; do zig build run -- "$f"; done
   ```

The resulting compiler binary is placed under `zig-out/bin`.

## Using the Compiler

Compile a `.dr` file directly:

```bash
./zig-out/bin/DreamCompiler path/to/file.dr
```

This writes `output.c` and builds a runnable program called `dream` in the current directory. Execute it with:

```bash
./dream
```

An example program is provided in [example.dr](example.dr).

## Documentation

Documentation for language features lives under [`docs`](docs). Start with [usage.md](docs/v1/usage.md) for an overview.

## Contributing

Contributions are welcome. Document any new language feature under `docs/` with a matching test case in `tests/`. Add additional dependencies to [`codex/_startup.sh`](codex/_startup.sh) if required.

## License

Dream Compiler is released under the [MIT License](LICENSE).
