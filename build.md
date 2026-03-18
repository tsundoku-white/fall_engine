## Linux (NixOS)
```bash
nix develop
cmake -B build && cmake --build build && ./build/app
```

## Linux (Ubuntu / Debian / Arch)
```bash
./install.sh
cmake -B build && cmake --build build && ./build/app
```

## Windows
1. Install [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)
2. Run `install.ps1` in PowerShell (installs vcpkg + deps)
3. Build:
```powershell
cmake -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```
