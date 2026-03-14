# fall_engine
This is a small engine the is written in cpp 23, this is just for fun.

## Dependencies

| Dependency | Purpose |
|------------|---------|
| Vulkan | Graphics API |
| GLFW3 | Window & input |
| GLM | Math library |
| stb_image | Image loading |
| cgltf | glTF model loading |
| fshader | Shader utilities |
| C++23+ | Language standard |
| CMake | Build system |
| Nix | Package management |

## Build

```bash
cd fall_engine
mkdir build && cd build
nix develop
cmake ..
make
./app
```

## Notes
- Nix is used for reproducible package management — run `nix develop` before configuring with CMake to enter the dev shell with all dependencies available.
- Requires a Vulkan-capable GPU and up-to-date drivers.
