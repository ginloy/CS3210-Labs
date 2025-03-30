{pkgs ? import <nixpkgs> {}}:
(pkgs.mkShell.override {stdenv = pkgs.gcc13Stdenv;}) {
  nativeBuildInputs = with pkgs; [
    texliveFull
    texlab
  ];
  buildInputs = with pkgs; [
    cudaPackages.cudatoolkit
    cudaPackages.cuda_cudart
    mpi
  ];
  CUDA_PATH = "${pkgs.cudaPackages.cudatoolkit}";
  EXTRA_LDFLAGS = "-L/lib";
  EXTRA_CCFLAGS = "-I/usr/include";
}
