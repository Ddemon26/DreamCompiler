# Dream Compiler

Dream is a minimal compiler that transforms a C#-like language into x86‑64 assembly. The project is experimental but aims to grow into a fully featured compiler. This repository contains the source for the compiler, example code, tests and documentation.

## Features

The compiler currently supports:

- Integer variable declarations and assignments
- Basic arithmetic using `+`
- Console output via `Console.WriteLine`

More features such as conditionals, loops and functions are planned for future versions. See the [changelog](docs/changelog.md) for the current status.

## Getting Started

The following guides explain how to set up and use Dream on Linux and Windows. They assume no prior software experience.

### Linux

1. **Install dependencies**

   ```bash
   sudo apt update
   sudo apt install -y build-essential cmake nasm gcc
   ```

2. **Clone the repository**

   ```bash
   git clone https://github.com/yourname/DreamCompiler.git
   cd DreamCompiler
   ```

3. **Build the compiler**

   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```

4. **Compile and run the example**

   ```bash
   ./DreamCompiler ../example.dr
   ./dream
   ```

5. **Run the tests (optional)**

   ```bash
   for f in ../tests/*.dr; do ./DreamCompiler $f; ./dream; done
   ```

### Windows

1. **Install tools**: [Git for Windows](https://git-scm.com/), [CMake](https://cmake.org/download/), [MinGW‑w64](http://mingw-w64.org/doku.php) and [NASM](https://www.nasm.us/). Make sure `gcc`, `cmake` and `nasm` are in your `PATH`.

2. **Clone the repository** using *Git Bash* or *Command Prompt*:

   ```cmd
   git clone https://github.com/Ddemon26/DreamCompiler.git
   cd DreamCompiler
   ```

3. **Build the compiler**

   ```cmd
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```

4. **Compile and run the example**

   ```cmd
   DreamCompiler ..\example.dr
   dream.exe
   ```

5. **Run the tests (optional)** following a similar loop in `cmd` or PowerShell.

## Building

Dream uses CMake. The quickest way to build on a Unix‑like system is:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

This produces the `DreamCompiler` executable inside the `build` directory.

## Using the Compiler

Invoke the compiler with a `.dr` source file:

```bash
./DreamCompiler path/to/file.dr
```

The compiler writes `output.asm` and assembles it into a runnable executable called `dream` in the current directory. Run the resulting program with:

```bash
./dream
```

An example program is provided in [example.dr](example.dr).

## Tests

Simple language features are covered by tests in the [`tests`](tests) directory. After building the compiler, run each test file through `DreamCompiler` and then execute the generated `dream` binary.

## Documentation

Documentation for individual language features lives under [`docs`](docs). Start with [usage.md](docs/usage.md) for a high level overview.

## Contributing

Contributions are welcome. Please keep code style simple and document any new language feature under `docs/` with a matching test case under `tests/`. Install additional dependencies in [`_startup.sh`](_startup.sh) if required.

## License

Dream Compiler is released under the [MIT License](LICENSE).
