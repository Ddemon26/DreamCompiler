# Dream Compiler

This section documents how to build and use the Dream compiler. It also covers the high level architecture of the project.

## Guides

- [IntelliSense Setup](intellisense.md)

Additional pages will be added here as the compiler grows.

## Debugging

Starting with version 1.1.07, the generated C code contains `#line` directives mapping back to the original `.dr` source. Compiler errors and debugger breakpoints therefore reference Dream source lines, making troubleshooting easier.
