#!/bin/bash

TARGET='qflipper'
BUILDDIR='build'

export QML_SOURCES_PATHS='..'

mkdir -p $BUILDDIR && cd $BUILDDIR
qmake ../$TARGET.pro -spec linux-g++ CONFIG+=qtquickcompiler && make qmake_all && make -j$(nproc)
linuxdeploy -e $TARGET -i ../$TARGET.png --appdir AppImage --plugin=qt --create-desktop-file -o appimage
