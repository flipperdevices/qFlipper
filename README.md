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
Put a firmware file named `f5_full.dfu` in the `FlipartnerGui` directory and then run 
```sh
./FlipartnerGui/FlipartnerGui
```

## Limitations:
* Fetching updates from remote server is not yet implemented
* Selecting local files is not yet implemented (see above)
* Some run-time errors are not handled correctly
