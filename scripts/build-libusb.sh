#!/bin/bash

TMP_DIR="/tmp/$(openssl rand -hex 3)";
LIBUSB_PATH="/opt/libs/libusb/1.0.24-universal";

build()
{
    local ARCH="$1";
    make clean;
    rm -rf "$TMP_DIR/libusb-$ARCH";
    rm -f Makefile;
    arch -"$ARCH" ./configure --prefix=/Users/build/temp/libusb-"$ARCH" CFLAGS="-mmacosx-version-min=10.14";
    arch -"$ARCH" make;
    arch -"$ARCH" make install;
}

join()
{
    lipo \
        -create \
        -output \
        "$LIBUSB_PATH/lib/libusb-1.0.0.dylib" \
        "$TMP_DIR/libusb-x86_64/lib/libusb-1.0.0.dylib" \
        "$TMP_DIR/libusb-arm64/lib/libusb-1.0.0.dylib";
}

build "x86_64";
build "arm64";
join;
rm -rf "$TMP_DIR";
