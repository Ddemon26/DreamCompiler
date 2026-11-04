# Multi-file regression

`main.dr` imports `math_utils.dr` and `string_utils.dr`, checks arithmetic and
recursion, and prints the result of an exported string function.

```sh
zig build compile-multi -- tests/multifile/main.dr
./dream
```

On Windows, execute `./dream.exe`. To run it as an output-checked regression:

```sh
python codex/test_cli.py run --pattern "multifile/main.dr"
```

The helper modules are fixtures excluded from standalone discovery by
`codex/test_config.json`. Additional module integration and rejection cases run
through the Go quick protocol in `codex/go/module_integration_test.go`.

See `docs/advanced-topics/modules.md` for implemented behavior and limits.
