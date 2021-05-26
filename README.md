# Flipartner
Flipper companion application (desktop)

## Prerequisites:
* Qt >= 5.12
* libusb >= 1.0.16

## Build:
```sh
mkdir build && cd build
qmake ../Flipartner.pro
make
```

## Run:
```sh
./FlipartnerGui/FlipartnerGui
```
or just launch the file above from your favourite file manager.

## Usage:
Click and hold the `Update` button until a menu appears. In the menu, select `Update from local file...`, pick the firmware file, then click `Yes`.

## Limitations:
* Fetching updates from remote server is not yet implemented
* Some run-time errors are not handled correctly
