# Supported Go CLI stability checks. Legacy cgo build targets are retired.
.DEFAULT_GOAL := quick-validate
.PHONY: quick-validate help

quick-validate:
	cd ../.. && zig build
	go test -count=1 -v -run 'TestCompiler_(DiagnosticExitStatus|ArenaGrowth|WarningsRemainNonfatal|Modules|LoopsAtOptimizationLevels|LiteralEmission|LiteralFailures|MethodArguments|ConstructorArguments|ParserRecovery|PointerParameters|IndexedStrings|LiteralStrings|IdentifierStrings|MemberStrings|ForStrings|CallStrings|AssignedStrings|StringContainers|LoopExitStrings|IndexedConsole|Dogfood)$$' .

help:
	@echo "make quick-validate (default): build the compiler and run CLI stability checks"
	@echo "See ../TEST_GUIDE.md; recursive legacy cgo tests are unsupported."
