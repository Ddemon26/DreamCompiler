# Supported Windows Go CLI checks; run from any working directory.
[CmdletBinding()]
param(
    [ValidateSet("quick", "help")]
    [string]$Action = "quick"
)

$ErrorActionPreference = "Stop"

if ($Action -eq "help") {
    Write-Output "Usage: build_and_test.ps1 [quick|help] (default: quick)"
    Write-Output "Builds DreamCompiler and runs the selected CLI stability checks."
    Write-Output "See codex/TEST_GUIDE.md; recursive legacy cgo tests are unsupported."
    exit 0
}

Get-Command zig, go -ErrorAction Stop | Out-Null

Push-Location (Join-Path $PSScriptRoot "../..")
try {
    & zig build
    $buildExit = $LASTEXITCODE
} finally {
    Pop-Location
}
if ($buildExit -ne 0) { exit $buildExit }

Push-Location $PSScriptRoot
try {
    & go test -count=1 -v -run 'TestCompiler_(DiagnosticExitStatus|ArenaGrowth|WarningsRemainNonfatal|Modules|LoopsAtOptimizationLevels|LiteralEmission|LiteralFailures|MethodArguments|ConstructorArguments|ParserRecovery|PointerParameters|IndexedStrings|LiteralStrings|IdentifierStrings|MemberStrings|ForStrings|CallStrings|AssignedStrings|StringContainers|LoopExitStrings|LoopExitClasses|ConditionalStrings|AssignmentInitializers|AppendedStrings|VoidReturnStrings|VoidReturnClasses|ScalarReturnCleanup|LocalStringReturn|LiteralStringReturn|ConditionalLiteralReturn|ConditionalLocalReturn|BorrowedParameterReturn|IndexedConsole|Dogfood)$' .
    $testExit = $LASTEXITCODE
} finally {
    Pop-Location
}
exit $testExit
