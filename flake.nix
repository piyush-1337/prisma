{
  description = "Prisma - media encoder/decoder";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
      lib = pkgs.lib;
      system = "x86_64-linux";
    in
    {
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "prisma";
        version = "0.1.0";

        src = lib.fileset.toSource {
          root = ./.;
          fileset = lib.fileset.unions [
            ./CMakeLists.txt
            ./src
            ./include
          ];
        };

        nativeBuildInputs = with pkgs; [
          cmake
          ninja
          cli11
        ];
      };

      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          gcc
          cmake
          ninja
          cli11
          zlib

          clang
          clang-tools
          neocmakelsp
        ];
      };
    };
}
