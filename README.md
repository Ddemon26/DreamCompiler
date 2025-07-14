# Dream Compiler

Dream is a minimal compiler that transforms a C#-like language into C code. The project is experimental but aims to grow into a fully featured compiler. This repository contains the source for the compiler, example code, tests and documentation.

## Features

The compiler currently supports:

- Integer variable declarations and assignments
- Basic arithmetic using `+`
- Console output via `Console.WriteLine`
- Simple `if` statements

More features such as conditionals, loops and functions are planned for future versions. See the [changelog](docs/changelog.md) for the current status.

## Getting Started

The following guides explain how to set up and use Dream on Linux and Windows. They assume no prior software experience.

### Linux

1. **Install dependencies**

   ```bash
   sudo apt update
   sudo apt install -y build-essential zig gcc
   ```

2. **Clone the repository**

   ```bash
   git clone https://github.com/yourname/DreamCompiler.git
   cd DreamCompiler
   ```

3. **Build the compiler**

   ```bash
   zig build
   ```

4. **Compile and run the example**

   ```bash
   zig build run -- example.dr
   ```

5. **Run the tests (optional)**

   ```bash
   for f in tests/*.dr; do zig build run -- $f; done
   ```

### Windows

1. **Install tools**: [Git for Windows](https://git-scm.com/) and [Zig](https://ziglang.org/download/). Ensure `zig` is available from the command prompt.

2. **Clone the repository** using *Git Bash* or *Command Prompt*:

   ```cmd
   git clone https://github.com/Ddemon26/DreamCompiler.git
   cd DreamCompiler
   ```

3. **Build the compiler**

   ```cmd
   zig build
   ```

4. **Compile and run the example**

   ```cmd
   zig build run -- example.dr
   ```

5. **Run the tests (optional)** using a simple loop in `cmd` or PowerShell.

## Building

Dream is built with Zig. The simplest approach is:

```bash
zig build
```

This places the `DreamCompiler` executable under `zig-out/bin`.

## Using the Compiler

Invoke the compiler with a `.dr` source file:

```bash
./zig-out/bin/DreamCompiler path/to/file.dr
```

The compiler writes `output.c` and compiles it into a runnable executable called `dream` in the current directory. Run the resulting program with:

```bash
./dream
```

An example program is provided in [example.dr](example.dr).

## Tests

Simple language features are covered by tests in the [`tests`](tests) directory. After building the compiler, run each test file using `zig build run -- <file>` which will compile the test and execute the resulting program.

## Documentation

Documentation for individual language features lives under [`docs`](docs). Start with [usage.md](docs/usage.md) for a high level overview.

## Contributing

Contributions are welcome. Please keep code style simple and document any new language feature under `docs/` with a matching test case under `tests/`. Install additional dependencies in [`_startup.sh`](_startup.sh) if required.

## License

Dream Compiler is released under the [MIT License](LICENSE).
