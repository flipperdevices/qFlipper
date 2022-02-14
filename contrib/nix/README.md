# Nix packaging for qFlipper


## Building

__Check that you have at least Nix 2.6.__

You can build qFlipper via Nix with one command.

```
nix build --experimental-features "nix-command flakes" 'github:flipperdevices/qFlipper?dir=contrib/nix&submodules=1'
```

For simplicity, I will leave out `--experimental-features "nix-command flakes"` in further examples. [Enable flakes](#Enabling%20flakes) to leave those options out.

If you wish to use a different version of qFlipper, just specify the commit hash or revision after the repo path.


```
nix build 'github:flipperdevices/qFlipper/COMMIT_HASH_OR_REVISION?dir=contrib/nix&submodules=1'
```

_Note that it won't work for revisions not containing Nix Flake files_

Built binaries will be located in `./result/bin` folder.

## Development

Run `nix develop 'contrib/nix?submodules=1'`
This will drop you in the shell with all required build dependencies.
After this, you can proceed as usual:
```
mkdir build
cd build
qmake ..
make -j30
```

You may also need to run `makeQtWrapper ./binary ./wrapper` on built binaries to generate wrapper shell script,
which would load all the required QT environment variables.


### Enabling flakes
If you wish to never again write `--experimental-features "nix-command flakes"`:
```
mkdir -p ~/.config/nix
echo "experimental-features = nix-command flakes" >> ~/.config/nix/nix.conf
```
