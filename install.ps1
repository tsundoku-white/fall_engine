# Clone vcpkg locally if not present
if (-Not (Test-Path "vcpkg")) {
    git clone https://github.com/microsoft/vcpkg.git
    .\vcpkg\bootstrap-vcpkg.bat -disableMetrics
}

# Install deps locally
.\vcpkg\vcpkg install glfw3 glm

Write-Host ""
Write-Host "Done! Build with:"
Write-Host "  cmake -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake"
Write-Host "  cmake --build build"
