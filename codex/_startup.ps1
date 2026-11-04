# Requires Chocolatey (https://chocolatey.org)
Write-Host "Installing Dream compiler dependencies..."

$packages = @(
    'git',
    'make',
    'mingw',
    're2c'
)

foreach ($pkg in $packages) {
    choco install $pkg -y
}

choco install zig --version=0.17.1 -y

Write-Host "Dependency setup complete ✅"
