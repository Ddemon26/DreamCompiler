#!/bin/bash
# _startup.sh: Install dependencies for the Dream compiler CLI agent

set -e

echo "Updating package lists..."
sudo apt update

echo "Installing core dependencies..."
sudo apt install -y build-essential curl

echo "Installing Zig..."
# Default to Zig 0.11 if ZIG_VERSION not provided
ZIG_VERSION=${ZIG_VERSION:-0.11.0}
ARCH=$(uname -m)
case "$ARCH" in
    x86_64)
        ZIG_ARCH="x86_64"
        ;;
    aarch64)
        ZIG_ARCH="aarch64"
        ;;
    *)
        echo "Unsupported architecture: $ARCH" >&2
        exit 1
        ;;
esac
ZIG_DIR="zig-linux-${ZIG_ARCH}-${ZIG_VERSION}"
ZIG_TAR="${ZIG_DIR}.tar.xz"

# Fetch and install Zig from the official release archive
curl -L "https://ziglang.org/download/${ZIG_VERSION}/${ZIG_TAR}" -o "/tmp/${ZIG_TAR}"
tar -C /tmp -xf "/tmp/${ZIG_TAR}"
sudo mv "/tmp/${ZIG_DIR}/zig" /usr/local/bin/
sudo chmod +x /usr/local/bin/zig
rm -rf "/tmp/${ZIG_DIR}" "/tmp/${ZIG_TAR}"

# Add additional dependencies below as needed

echo "Cleaning up..."
sudo apt autoremove -y

echo "Dependency setup complete."
