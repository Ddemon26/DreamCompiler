#!/bin/bash
# _startup.sh: Manages dependencies for the Dream compiler CLI agent

echo "Updating package lists..."
sudo apt update

echo "Installing core dependencies..."
sudo apt install -y gcc nasm binutils

# Add new dependencies here as needed by the agent
# Example: sudo apt install -y make

echo "Removing unused dependencies..."
sudo apt autoremove -y

echo "Dependency setup complete."