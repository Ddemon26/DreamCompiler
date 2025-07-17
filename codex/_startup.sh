#!/usr/bin/env bash
# _startup.sh: Install dependencies for the Dream compiler CLI agent
#
# Supported distros: Debian/Ubuntu (apt)
# Installs: build‑essential, curl, git, gdb, re2c, Zig

set -euo pipefail

echo "Updating package lists…"
sudo apt update

###############################################################################
# Core build/debug dependencies
###############################################################################
echo "Installing core dependencies (build-essential, curl, git, gdb)…"
sudo apt install -y build-essential curl git gdb

###############################################################################
# 1. Install re2c (required for lexer generation)
###############################################################################
if apt-cache show re2c >/dev/null 2>&1; then
  echo "Installing re2c from the distro repository…"
  sudo apt install -y re2c
else
  echo "re2c not in repository – building latest release from source…"
  # grab the most recent tag name from GitHub (requires git & curl)
  RE2C_TAG=$(curl -s https://api.github.com/repos/skvadrik/re2c/releases/latest \
              | grep -oP '"tag_name":\s*"\K(.*)(?=")')
  curl -L "https://github.com/skvadrik/re2c/archive/${RE2C_TAG}.tar.gz" -o /tmp/re2c.tar.gz
  tar -C /tmp -xf /tmp/re2c.tar.gz
  pushd "/tmp/re2c-${RE2C_TAG}"
    ./autogen.sh
    ./configure
    make -j"$(nproc)"
    sudo make install
  popd
  rm -rf "/tmp/re2c-${RE2C_TAG}" /tmp/re2c.tar.gz
fi

###############################################################################
# 2. Install Zig
###############################################################################
echo "Installing Zig…"
ZIG_VERSION=${ZIG_VERSION:-0.15.0-dev.1034+bd97b6618}
ARCH=$(uname -m)
case "$ARCH" in
  x86_64)  ZIG_ARCH="x86_64-linux"  ;;
  aarch64) ZIG_ARCH="aarch64-linux" ;;
  *)
    echo "Unsupported architecture: $ARCH" >&2
    exit 1
    ;;
esac
ZIG_DIR="zig-${ZIG_ARCH}-${ZIG_VERSION}"
ZIG_TAR="${ZIG_DIR}.tar.xz"

if [[ "$ZIG_VERSION" == *"dev"* || "$ZIG_VERSION" == *"rc"* ]]; then
  BASE_URL="https://ziglang.org/builds"
else
  BASE_URL="https://ziglang.org/download/${ZIG_VERSION}"
fi

curl -L "${BASE_URL}/${ZIG_TAR}" -o "/tmp/${ZIG_TAR}"
tar -C /tmp -xf "/tmp/${ZIG_TAR}"
sudo mv "/tmp/${ZIG_DIR}" /usr/local/zig
sudo ln -sf /usr/local/zig/zig /usr/local/bin/zig
sudo chmod +x /usr/local/zig/zig
rm -f "/tmp/${ZIG_TAR}"

###############################################################################
# 3. Clean‑up
###############################################################################
echo "Cleaning up…"
sudo apt autoremove -y
echo "Dependency setup complete ✅"