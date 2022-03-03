#!/bin/bash

set -ex

TARGET='qFlipper'
BUILDDIR='build'
APPDIR_PREFIX="$PWD/$BUILDDIR/AppDir/usr"
#LIBUSB_OVERRIDE="$(ldconfig -p | grep x86-64 | grep -oP '/[^\s]+/libusb-1.0\.so\.\d+$' | head -n1)"

export OUTPUT=$TARGET-x86_64.AppImage
export QML_SOURCES_PATHS='..'

mkdir -p $BUILDDIR && cd $BUILDDIR
qmake ../$TARGET.pro -spec linux-g++ CONFIG+=qtquickcompiler PREFIX=$APPDIR_PREFIX && make qmake_all && make -j$(nproc) && make install
linuxdeploy --appdir=AppDir -o appimage --custom-apprun=../installer-assets/appimage/AppRun --plugin=qt 
