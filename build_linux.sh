#!/bin/bash

set -ex

TARGET='qFlipper'
BUILDDIR='build'
APPDIR_PREFIX="$PWD/$BUILDDIR/AppDir/usr"
LIBSSL1_OVERRIDE="$(ldconfig -p | grep x86-64 | grep -oP '/[^\s]+/libssl.so.1.1' | head -n1)"

export OUTPUT=$TARGET-x86_64.AppImage
export QML_SOURCES_PATHS='..'

# debug
echo "Testing Boost version:";
cat > boost-ver.cpp << EOF
#include <iostream>
#include <boost/version.hpp>

int main(void)
{
    std::cout << "Using Boost "
        << BOOST_VERSION / 100000 << "."
        << BOOST_VERSION / 100 % 1000 << "."
        << BOOST_VERSION % 100
        << std::endl;
    return(0);
}
EOF
g++ boost-ver.cpp -o boost-ver && ./boost-ver || true;

echo "Testing problem files type:";
file /lib/x86_64-linux-gnu/libdbus-1.so.3;
ls -l /lib/x86_64-linux-gnu/libdbus-1.so.3;
readlink -f /lib/x86_64-linux-gnu/libdbus-1.so.3;
file $(readlink -f /lib/x86_64-linux-gnu/libdbus-1.so.3);

echo "Example normal file:"
file /lib/x86_64-linux-gnu/libbsd.so.0;
ls -l /lib/x86_64-linux-gnu/libbsd.so.0;
readlink -f /lib/x86_64-linux-gnu/libbsd.so.0;
file $(/lib/x86_64-linux-gnu/libbsd.so.0);
# debug

mkdir -p $BUILDDIR && cd $BUILDDIR
qmake ../$TARGET.pro -spec linux-g++ CONFIG+=qtquickcompiler PREFIX=$APPDIR_PREFIX && make qmake_all && make -j$(nproc) && make install
linuxdeploy --appdir=AppDir -o appimage --custom-apprun=../installer-assets/appimage/AppRun --plugin=qt --library=$LIBSSL1_OVERRIDE
