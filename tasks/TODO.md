# Outstanding Tasks

The grammar specification in `docs/grammar/Grammar.md` lists several language features such as loops, switch statements, and return/break/continue statements as *implemented*. The current parser now supports `while` loops in addition to variable declarations, `if`/`else` statements, and expression statements.

Future work:
- Implement parsing and code generation for remaining loops (`do`, `for`).
- Add support for `switch` statements, `break`, `continue`, and `return`.
- Extend the AST and code generator accordingly.
- Update the grammar once features land to keep it authoritative.
