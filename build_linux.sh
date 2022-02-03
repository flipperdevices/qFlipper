#!/bin/bash

set -ex

TARGET='qFlipper'
BUILDDIR='build'

export OUTPUT=$TARGET-x86_64.AppImage
export QML_SOURCES_PATHS='..'

mkdir -p $BUILDDIR && cd $BUILDDIR
qmake ../$TARGET.pro -spec linux-g++ CONFIG+=qtquickcompiler && make qmake_all && make -j$(nproc)
linuxdeploy --appdir=AppDir -o appimage -e ${TARGET}Tool -e $TARGET \
            --custom-apprun=../installer-assets/appimage/AppRun --plugin=qt \
            -d ../installer-assets/appimage/qFlipper.desktop \
            -i ../application/assets/icons/qFlipper.png
