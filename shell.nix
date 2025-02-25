{ pkgs ? import <unstable> {} }:
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    texliveFull
    texlab
  ];
}
