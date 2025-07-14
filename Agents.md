Dream Compiler CLI Agent
Overview
The Dream Compiler CLI Agent is responsible for iteratively developing and maintaining the Dream compiler, a tool that compiles the Dream programming language (.dr files) with C#-like, semicolon-based syntax into x86-64 assembly (NASM format). The agent operates in a Windows environment using CLion, MinGW, and NASM, ensuring continuous progress toward a complete compiler. The agent responds to the command go by performing as much work as possible, including implementing new features, updating documentation, and running tests until all pass.
Responsibilities
The agent’s primary goal is to create a robust compiler for the Dream language, supporting C#-like syntax (e.g., int x = 5;, x = y + z;, Console.WriteLine(x);). It must:

Iteratively implement new features in dream.c, expanding the language’s capabilities.
Generate x86-64 assembly compatible with Windows (using printf and ExitProcess) or Linux (via WSL, using syscalls).
Maintain a /docs directory with Markdown files documenting each feature and its syntax usage.
Maintain a /tests directory with test cases for each feature, ensuring all tests pass before stopping.
Continue development until the compiler supports a comprehensive set of features (e.g., variables, arithmetic, control flow, functions).
Handle errors gracefully and provide clear feedback in CLion or the terminal.

Workflow
When the command go is issued, the agent:
add any dependencies to the _startup.sh file for next cli agent run

Assesses Current State:
Reviews dream.c for existing functionality.
Checks /docs for documented features and usage guides.
Scans /tests for test cases and their results.


Implements New Features:
Prioritizes features like conditionals (if), loops (while, for), and functions, based on C# syntax.
Updates the lexer (next_token), parser (parse_statement), and codegen (generate_assembly) in dream.c.
Ensures Windows-compatible assembly (e.g., using printf for output).


Updates Documentation:
Creates or updates /docs/<feature>.md for each new feature, detailing syntax and usage examples.
Maintains /docs/usage.md as a comprehensive guide for the Dream language.


Creates and Runs Tests:
Adds test cases in /tests/<feature>.dr for each new feature.
Runs tests by compiling test files with dream.exe, assembling with NASM, linking with MinGW GCC, and executing the output.
Continues iterating until all tests pass (e.g., correct output for arithmetic, control flow).


Builds and Tests:
Builds the compiler in CLion (Ctrl+F9) or via cmake --build ..
Runs dream.exe <test>.dr to generate output.asm.
Assembles and links: nasm -f win64 output.asm -o output.obj && gcc output.obj -o dream_exec.exe.
Executes dream_exec.exe and verifies output against expected results.


Commits Changes:
Uses Git to commit changes to dream.c, /docs, and /tests.
Maintains a changelog in /docs/changelog.md.



Constraints

Environment: Runs in CLion on Windows, using MinGW for compiling dream.c and NASM for assembling output.asm.
Persistence: Continues development until the compiler supports a full C#-like feature set or is explicitly stopped.
Testing: Does not stop until all tests in /tests pass, verified by running generated executables.
Documentation: Every feature must have a corresponding /docs/<feature>.md file and be reflected in /docs/usage.md.
Error Handling: Reports syntax errors, compilation failures, or test failures clearly in CLion or terminal output.

Trigger

Command: go
Action: The agent performs all necessary tasks (implement, document, test) without further input, stopping only when all tests pass or a critical error requires user intervention.

Directory Structure

/docs: Contains Markdown files for each feature and a general usage guide.
variables.md: Documents variable declarations and assignments.
arithmetic.md: Documents arithmetic operations.
console.md: Documents Console.WriteLine.
usage.md: Comprehensive syntax guide.
changelog.md: Tracks updates.


/tests: Contains .dr test files for each feature.
test_variables.dr: Tests variable declarations and assignments.
test_arithmetic.dr: Tests arithmetic operations.
test_console.dr: Tests Console.WriteLine.



Initial Features
The agent starts with the following implemented features:

Variable declarations (int x;)
Assignments (x = 5;)
Basic arithmetic (x = y + z;)
Console output (Console.WriteLine(x);)

Future Features
The agent will prioritize:

Conditionals (if, else)
Loops (while, for)
Functions (void MyFunction() { ... })
String output for Console.WriteLine (formatting numbers as strings)

Example Execution
> go
[Agent] Assessing current state...
[Agent] Implementing 'if' statement...
[Agent] Updating /docs/if.md...
[Agent] Creating /tests/test_if.dr...
[Agent] Building dream.c...
[Agent] Running tests...
[Agent] All tests passed!
[Agent] Committing changes...

The agent ensures the Dream compiler evolves into a robust tool, maintaining C#-like syntax and reliable assembly output.