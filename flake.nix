{
  description = "Vulkan C++ application with GLM and STB Image";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        
        # STB Image is header-only, so we'll just ensure it's available
        stb-image = pkgs.fetchFromGitHub {
          owner = "nothings";
          repo = "stb";
          rev = "f54acd4e13430c5122cab4ca657705c84aa61b08"; # Latest as of 2024
          hash = "sha256-Rucm8op04aQVMx6tEgLUbmVe8/ZuHNZ9tE6KhwPEcLQ=";
        };
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "vulkan-app";
          version = "0.1.0";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            pkg-config
            shaderc
          ];

          buildInputs = with pkgs; [
            glfw
            vulkan-headers
            vulkan-loader
            vulkan-validation-layers
            glm
            stb
            
            xorg.libX11
            xorg.libXrandr
            xorg.libXinerama
            xorg.libXcursor
            xorg.libXi
          ];

          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=Debug"
            "-DSTB_INCLUDE_DIR=${stb-image}"
          ];

          installPhase = ''
            mkdir -p $out/bin
            cp app $out/bin/
            
            # Copy compiled shaders
            if [ -d "shaders" ]; then
              mkdir -p $out/share/shaders
              cp -r shaders/* $out/share/shaders/
            fi
          '';
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            cmake
            pkg-config
            glfw
            vulkan-headers
            vulkan-loader
            vulkan-validation-layers
            vulkan-tools
            shaderc
            glm
            
            xorg.libX11
            xorg.libXrandr
            xorg.libXinerama
            xorg.libXcursor
            xorg.libXi
            
            # LSP and development tools
            clang-tools
            
            # Formatters
            nixpkgs-fmt
            cmake-format
          ]; 

          # Environment variables
          PKG_CONFIG_PATH = "${pkgs.glfw}/lib/pkgconfig:${pkgs.vulkan-loader}/lib/pkgconfig";
          CPATH = "${pkgs.glfw}/include:${pkgs.vulkan-headers}/include:${pkgs.glm}/include";
          VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
          LD_LIBRARY_PATH = "${pkgs.vulkan-loader}/lib:${pkgs.vulkan-validation-layers}/lib";
        };
      }
    );
}
