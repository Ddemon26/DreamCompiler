# Dream Compiler

This section documents how to build and use the Dream compiler. It also covers the high level architecture of the project.

## Guides

- [IntelliSense Setup](intellisense.md)

Additional pages will be added here as the compiler grows.

## Debugging

Starting with version 1.1.07, the generated C code contains `#line` directives mapping back to the original `.dr` source. As of 1.1.12 these directives are emitted before every statement and the C compiler is invoked with `-g`, so debuggers step through the original Dream source.
