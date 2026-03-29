{
  description = "icey packaged as a Nix flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        version = builtins.replaceStrings ["\n" "\r"] ["" ""] (builtins.readFile ./VERSION);
        icey = pkgs.callPackage ./packaging/nix/package.nix {
          inherit version;
          src = self;
        };
        iceyMinimal = pkgs.callPackage ./packaging/nix/package.nix {
          inherit version;
          src = self;
          withFfmpeg = false;
        };
      in {
        packages.default = icey;
        packages.icey = icey;
        packages.icey-minimal = iceyMinimal;

        devShells.default = pkgs.mkShell {
          inputsFrom = [ icey ];
          packages = with pkgs; [
            cmake
            pkg-config
          ];
        };
      });
}
