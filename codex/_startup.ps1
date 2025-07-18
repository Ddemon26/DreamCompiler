# Requires Chocolatey (https://chocolatey.org)
Write-Host "Installing Dream compiler dependencies..."

$packages = @(
    'git',
    'make',
    'mingw',
    'gdb',
    're2c',
    'zig'
)

foreach ($pkg in $packages) {
    choco install $pkg -y
}

Write-Host "Dependency setup complete ✅"
