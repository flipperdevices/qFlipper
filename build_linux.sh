#!/bin/bash

PROJECT='Flipartner'
TARGET=$(echo $PROJECT | tr '[:upper:]' '[:lower:]')
BUILDDIR='build'

export QML_SOURCES_PATHS='..'

mkdir -p $BUILDDIR && cd $BUILDDIR
qmake ../$PROJECT.pro -spec linux-g++ CONFIG+=qtquickcompiler && make qmake_all && make -j$(nproc)
linuxdeploy -e $TARGET -i ../$TARGET.png --appdir AppImage --plugin=qt --create-desktop-file -o appimage
