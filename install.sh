#!/usr/bin/env bash
set -e

echo "==> Detecting environment..."

# NixOS / Nix available
if command -v nix &>/dev/null; then
    echo "Nix detected. Run 'nix develop' then 'cmake -B build && cmake --build build'"
    exit 0
fi

# Ubuntu / Debian
if command -v apt &>/dev/null; then
    echo "==> Installing deps via apt..."
    sudo apt update
    sudo apt install -y \
        build-essential cmake pkg-config \
        libvulkan-dev vulkan-tools glslc \
        libglfw3-dev libglm-dev \
        libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev

# Arch
elif command -v pacman &>/dev/null; then
    echo "==> Installing deps via pacman..."
    sudo pacman -S --noconfirm \
        base-devel cmake pkgconf \
        vulkan-devel vulkan-tools shaderc \
        glfw glm \
        libx11 libxrandr libxinerama libxcursor libxi

else
    echo "Unsupported distro. Please install dependencies manually (see README)."
    exit 1
fi

echo ""
echo "==> Done! Build with:"
echo "    cmake -B build && cmake --build build && ./build/app"
