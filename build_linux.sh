#!/bin/bash

set -euxo pipefail;

TARGET="qFlipper"
BUILDDIR="build"
APPDIR_PREFIX="$PWD/$BUILDDIR/AppDir/usr"

LIBSSL1_OVERRIDE="$(ldconfig -p | grep x86-64 | grep -oP '/[^\s]+/libssl.so.1.1' | head -n1)"

LIBWAYLAND_EXCLUDE="libwayland*"
LIBXCB_EXCLUDE="libxcb*"
LIBXKB_EXCLUDE="libxkb*"
LIBX11_EXCLUDE="libX*"

export OUTPUT="$TARGET-x86_64.AppImage"

mkdir -p "$BUILDDIR" && cd "$BUILDDIR"

qmake "../$TARGET.pro" -spec linux-g++ "CONFIG+=release qtquickcompiler" PREFIX="$APPDIR_PREFIX"
make qmake_all
make -j"$(nproc)"
make install

linuxdeploy --appdir=AppDir -o appimage \
    --custom-apprun="../installer-assets/appimage/AppRun" \
    --library="$LIBSSL1_OVERRIDE" \
    --exclude-library="$LIBWAYLAND_EXCLUDE" \
    --exclude-library="$LIBXCB_EXCLUDE" \
    --exclude-library="$LIBXKB_EXCLUDE" \
    --exclude-library="$LIBX11_EXCLUDE"
