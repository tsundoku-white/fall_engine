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

## Progress

- [x] Input
- [x] Vulkan renderer
- [ ] Camera system
- [ ] Lighting & shadows
- [ ] Model loading (glTF)
- [ ] Texture loading
      
## Build
See the [build](build.md) file for manual build instructions, 
or run `install.sh` for automatic setup.

## Notes
- Requires a Vulkan-capable GPU and up-to-date drivers.
- Run `nix develop` before CMake to enter the dev shell with all dependencies.
