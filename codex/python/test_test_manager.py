"""Regression tests for suite outcomes that previously hid or misreported tests."""
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest.mock import Mock, patch

import test_manager as runner


class TestRunnerOutcomes(unittest.TestCase):
    def setUp(self):
        self.manager = runner.TestManager()
        self.manager.logger = Mock()
        self.workspace = tempfile.TemporaryDirectory(dir=runner.RESULTS_DIR)
        self.addCleanup(self.workspace.cleanup)
        self.root = Path(self.workspace.name)

    def fixture(self, content):
        path = self.root / "input.dr"
        path.write_text(content, encoding="utf-8")
        return path

    def test_explicit_empty_output_runs(self):
        path = self.fixture("// Expected: (no output)\nint x = 1;")
        with patch.object(self.manager, "compile_test", return_value=(True, 0.0, "")), \
                patch.object(self.manager, "execute_test", return_value=(True, 0.0, "")) as execute:
            result = self.manager.run_single_test(path)
        self.assertEqual(result.status, runner.TestStatus.PASS)
        execute.assert_called_once()

    def test_expected_diagnostic_requires_compiler_failure(self):
        path = self.fixture("// Expected error: undefined variable\nint x = missing;")
        for success, status in ((False, runner.TestStatus.PASS), (True, runner.TestStatus.FAIL)):
            with self.subTest(compile_success=success), \
                    patch.object(self.manager, "compile_test", return_value=(success, 0.0, "undefined variable")) as compile_test, \
                    patch.object(self.manager, "execute_test") as execute:
                result = self.manager.run_single_test(path)
                self.assertEqual(result.status, status)
                compile_test.assert_called_once_with(path, [], link=False)
                execute.assert_not_called()

    def test_expected_exit_preserves_output_validation(self):
        path = self.fixture("// Expected: result\n// Expected exit: 8\n")
        for name in ("dream", "dream.exe"):
            (self.root / name).touch()
        process = subprocess.CompletedProcess([], 8, stdout="result\n", stderr="")
        with patch.object(runner, "ROOT", self.root), \
                patch.object(self.manager, "compile_test", return_value=(True, 0.0, "")), \
                patch.object(self.manager, "run_command", return_value=process):
            self.assertEqual(self.manager.run_single_test(path).status, runner.TestStatus.PASS)
            process.stdout = "wrong output\n"
            self.assertEqual(self.manager.run_single_test(path).status, runner.TestStatus.FAIL)

    def test_timeout_fails_suite_and_continues(self):
        path = self.fixture("// Expected: done\n")
        passed = runner.TestResult(
            name="next.dr", path="next.dr", status=runner.TestStatus.PASS,
            category=runner.TestCategory.UNIT, duration=0.0, expected_output="done",
            actual_output="done", compile_time=0.0, runtime=0.0,
        )
        with patch.object(self.manager, "discover_tests", return_value=[path, path]), \
                patch.object(self.manager, "run_single_test", side_effect=[subprocess.TimeoutExpired(["compiler"], 1), passed]), \
                patch.object(self.manager, "save_results"), patch.object(self.manager, "print_summary"):
            suite = self.manager.run_test_suite()
        self.assertEqual(suite.errors, 1)
        self.assertEqual(suite.passed, 1)
        self.assertEqual(suite.total, 2)

    def test_module_fixtures_are_not_counted_as_tests(self):
        (self.root / "main.dr").touch()
        (self.root / "helper.dr").touch()
        self.manager.config["fixture_files"] = ["helper.dr"]
        with patch.object(runner, "TEST_DIR", self.root):
            self.assertEqual(self.manager.discover_tests(), [self.root / "main.dr"])


if __name__ == "__main__":
    unittest.main()
