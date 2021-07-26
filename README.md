# qFlipper

Flipper companion application (desktop)

## Build:
### Windows:

Build requirements:
- MS Visual Studio 2017 or newer
- Qt (MSVC build) >= 5.12.0
- Internet connection (to download OpenSSL files)

Edit `build_windows.bat` to adjust to your build environment and then run:
```cmd
build_windows.bat
```

### Linux:

```sh
docker-compose exec dev ./build_linux.sh
```

### MacOS:

Build requirements:

- Xcode or command line tools
- Qt
- Libusb

If you want to sign binaries, set `SIGNING_KEY` environment variable:

	export SIGNING_KEY="Your Developer Key ID"

Building, signing and creating package:

	./build_mac.sh

Resulting image can be found in: `build_mac/qFlipper.dmg`

## Run:

### Linux:
```sh
./build/qFlipper-x86_64.AppImage
```

or just launch the file above from your favourite file manager.

## Usage:

Please take a look at the new [qFlipper wiki](https://github.com/Flipper-Zero/qFlipper/wiki)!

## Limitations:

* Some run-time errors are not handled correctly
