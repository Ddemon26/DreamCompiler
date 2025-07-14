#!/bin/bash
# _startup.sh: Install dependencies for the Dream compiler CLI agent

set -e

echo "Updating package lists..."
sudo apt update

echo "Installing core dependencies..."
sudo apt install -y build-essential zig

# Add additional dependencies below as needed

echo "Cleaning up..."
sudo apt autoremove -y

echo "Dependency setup complete."
