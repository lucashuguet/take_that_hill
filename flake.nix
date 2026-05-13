{
  description = "take that hill in cpp";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { nixpkgs, ... }: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
    pname = "takethathill";
    version = "0.1.0";
    src = ./.;
    buildInputs = [ ];
    cmakeFlags = [ "-DAPP_NAME=${pname}" ];
    nativeBuildInputs = with pkgs; [ makeWrapper cmake ninja ];
    devInputs = with pkgs; [ gdb clang-tools ];
  in rec {
    devShells.${system}.default = pkgs.mkShell {
      inherit buildInputs;
      nativeBuildInputs = nativeBuildInputs ++ devInputs;
      shellHook = ''
        cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ${pkgs.lib.concatStringsSep " " cmakeFlags}
        mv build/compile_commands.json .
        rm -rf build/*
        mv compile_commands.json build/
      '';
    };

    packages.${system} = {
      default = pkgs.stdenv.mkDerivation {
        inherit buildInputs cmakeFlags nativeBuildInputs pname version src;
        postFixup = ''
          wrapProgram $out/bin/${pname} \
            --prefix PATH : ${pkgs.lib.makeBinPath buildInputs}
        '';
      };

      debug = pkgs.stdenv.mkDerivation {
        inherit buildInputs nativeBuildInputs pname version src;
        cmakeFlags = cmakeFlags ++ [ "-DCMAKE_BUILD_TYPE=Debug" ];
        dontStrip = true;
        postFixup = ''
          wrapProgram $out/bin/${pname} \
            --prefix PATH : ${pkgs.lib.makeBinPath buildInputs}
        '';
      };

      test = pkgs.writeShellScriptBin pname ''
        export PATH="${pkgs.lib.makeBinPath buildInputs}:$PATH"
        ${pkgs.valgrind}/bin/valgrind --leak-check=full --error-exitcode=1 \
          ${packages.${system}.debug}/bin/.${pname}-wrapped "$@"
      '';

      gdb = pkgs.writeShellScriptBin pname ''
        export PATH="${pkgs.lib.makeBinPath buildInputs}:$PATH"
        ${pkgs.gdb}/bin/gdb ${packages.${system}.debug}/bin/.${pname}-wrapped
      '';
    };
  };
}
