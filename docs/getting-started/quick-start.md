# Your first Dream program

Write a small program, compile it to C, and run the native executable. Dream is
experimental; this guide uses a deliberately small part of the language.

## Set up the toolchain

Install Git, **Zig 0.17.1**, and **re2c**, with `zig` and `re2c` on your `PATH`.
See the [repository setup instructions](../../README.md#getting-started) for
platform prerequisites. Python and Go are needed for the project's test workflows,
not for these two examples.

For a new checkout of the development version described by this site:

```sh
git clone --branch codex/compiler-stabilization https://github.com/Ddemon26/DreamCompiler.git
cd DreamCompiler
zig version
zig build
```

The version command must print `0.17.1`. If you already have a checkout, build from
its root. Follow the [build guide](../development/compiler/build.md) for other
build options and bootstrap details.

## Start with hello

Create `hello.dr` in the repository root:

```dream
// Every idea starts somewhere.
string message = "Hello, Dream!";
Console.WriteLine(message);
```

Dream permits top-level statements. This example declares a string and prints it.

## Compile and run

From the repository root:

```sh
zig build run -- hello.dr
```

This compiles the source; run the generated executable separately.

On Linux:

```sh
./dream
```

On Windows, in PowerShell:

```powershell
.\dream.exe
```

Expected output:

```text
Hello, Dream!
```

Compilation writes `build/bin/dream.c` and the `dream` or `dream.exe` executable
in the working directory. The executable is not named after your source file.

## Add a function

Replace the contents of `hello.dr` with:

```dream
func int add(int a, int b) {
    return a + b;
}

Console.WriteLine(add(5, 3));
```

Compile and run again using the same commands. Expected output:

```text
8
```

The function takes two integers and returns an integer. Read the
[functions guide](../language-features/functions.md) for the syntax and
qualified return checks.

## Keep exploring

- [Types and variables](../fundamentals/variables.md)
- [Control flow](../language-features/language-guide/control-flow.md)
- [Function modules](../advanced-topics/modules.md)
- [Implementation status](../development/implementation-status.md)

Older tutorials and version archives can include planned or unqualified behavior.
The implementation notes link to the current feature inventory and regression
evidence so you can check a feature before relying on it.
