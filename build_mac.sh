#!/bin/bash

set -e
set -x

PROJECT_DIR=`pwd`
PROJECT="qflipper"
BUILD_DIRECTORY="build_mac"

if [[ -d "$BUILD_DIRECTORY" ]]
then
	rm -rf "$BUILD_DIRECTORY"
fi

mkdir "$BUILD_DIRECTORY"
cd "$BUILD_DIRECTORY"

qmake -spec macx-clang CONFIG+=release CONFIG+=x86_64 -o Makefile ../$PROJECT.pro
make -j9 > /dev/null
macdeployqt $PROJECT.app -qmldir=$PROJECT_DIR/Application -verbose=1

FAILED_LIBS_COUNT=`otool -L $PROJECT.app/Contents/Frameworks/*.dylib | grep /usr/local -c || true`

if [[ $FAILED_LIBS_COUNT -gt 0 ]]
then
	echo "Not all libraries use proper paths"
	exit 255
fi

# Sign
if [ -n "$SIGNING_KEY" ]
then
	xattr -cr $PROJECT.app
	codesign --force -s "$SIGNING_KEY" --deep -v $PROJECT.app
fi

# build DMG
mkdir disk_image
ln -s /Applications disk_image/Applications
mv $PROJECT.app disk_image
hdiutil create -volname $PROJECT -srcfolder disk_image -ov -format UDZO $PROJECT.dmg
