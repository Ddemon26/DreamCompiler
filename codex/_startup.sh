#!/bin/bash
# _startup.sh: Install dependencies for the Dream compiler CLI agent

set -e

echo "Updating package lists..."
sudo apt update

echo "Installing core dependencies..."
sudo apt install -y build-essential curl

echo "Installing Zig..."
# Default to Zig 0.15 if ZIG_VERSION not provided
ZIG_VERSION=${ZIG_VERSION:-0.15.0-dev.1034+bd97b6618}
ARCH=$(uname -m)
case "$ARCH" in
    x86_64)
        ZIG_ARCH="x86_64-linux"
        ;;
    aarch64)
        ZIG_ARCH="aarch64-linux"
        ;;
    *)
        echo "Unsupported architecture: $ARCH" >&2
        exit 1
        ;;
esac
ZIG_DIR="zig-${ZIG_ARCH}-${ZIG_VERSION}"
ZIG_TAR="${ZIG_DIR}.tar.xz"

# Determine base URL. Development builds are hosted under /builds
if [[ "$ZIG_VERSION" == *"dev"* || "$ZIG_VERSION" == *"rc"* ]]; then
    BASE_URL="https://ziglang.org/builds"
else
    BASE_URL="https://ziglang.org/download/${ZIG_VERSION}"
fi

# Fetch and install Zig from the official release archive
curl -L "${BASE_URL}/${ZIG_TAR}" -o "/tmp/${ZIG_TAR}"
tar -C /tmp -xf "/tmp/${ZIG_TAR}"
sudo mv "/tmp/${ZIG_DIR}" /usr/local/zig
sudo ln -sf /usr/local/zig/zig /usr/local/bin/zig
sudo chmod +x /usr/local/zig/zig
rm -rf "/tmp/${ZIG_TAR}"

# Add additional dependencies below as needed

echo "Cleaning up..."
sudo apt autoremove -y

echo "Dependency setup complete."
