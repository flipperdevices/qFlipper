{
  inputs = {
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { nixpkgs, self, utils }: utils.lib.eachDefaultSystem (system:
    let
    pkgs = import nixpkgs { inherit system; };
    in
    {
      defaultPackage = pkgs.libsForQt515.callPackage ./. { };
      nixpkgs = pkgs;
    });

}
