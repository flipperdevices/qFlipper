#!/bin/bash

set -euxo pipefail;

TARGET="qFlipper"
BUILDDIR="build"
APPDIR_PREFIX="$PWD/$BUILDDIR/AppDir/usr"
LIBSSL1_OVERRIDE="$(ldconfig -p | grep x86-64 | grep -oP '/[^\s]+/libssl.so.1.1' | head -n1)"

export OUTPUT="$TARGET-x86_64.AppImage"
export QML_SOURCES_PATHS=".."

mkdir -p "$BUILDDIR"
cd "$BUILDDIR"
qmake i"../$TARGET.pro" -spec linux-g++ CONFIG+=qtquickcompiler PREFIX="$APPDIR_PREFIX"
make qmake_all
make -j"$(nproc)"
make install
linuxdeploy --appdir=AppDir -o appimage --custom-apprun="../installer-assets/appimage/AppRun" --plugin=qt --library="$LIBSSL1_OVERRIDE"
