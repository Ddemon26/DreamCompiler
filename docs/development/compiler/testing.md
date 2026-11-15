# Testing the compiler

The maintained command reference is the
[DreamCompiler testing guide](../../../codex/TEST_GUIDE.md). It covers full and
focused Dream runs, annotated expectations, runner regressions, Go quick checks,
shared build outputs and native versus cross-build evidence.

The working paths are:

- [Python CLI](../../../codex/test_cli.py) and
  [runner](../../../codex/python/test_manager.py).
- [Runner regressions](../../../codex/python/test_test_manager.py).
- [Test configuration](../../../codex/test_config.json) and
  [Dream fixtures](../../../tests).
- [Go CLI checks](../../../codex/go/README.md).
- [CI workflow](../../../.github/workflows/ci.yml).
- [Dated results and remaining skips](../../../tasks/MostRecentTestResults.md).

Older timing promises, sample runners and recursive cgo test commands have been
removed from this page. Current compiler behavior is established by the dated
checks and source, not by historical test totals or examples alone.
