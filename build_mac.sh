#!/bin/bash

set -e
set -x

PROJECT_DIR=`pwd`
PROJECT="qFlipper"
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

# Add the Tool to bundle
cp ${PROJECT}Tool $PROJECT.app/Contents/MacOS
# Fix libraries for the Tool
install_name_tool -change /usr/local/opt/libusb/lib/libusb-1.0.0.dylib @executable_path/../Contents/Frameworks/libusb-1.0.0.dylib $PROJECT.app/Contents/MacOS/${PROJECT}Tool
install_name_tool -change /usr/local/opt/qt@5/lib/QtSerialPort.framework/Versions/5/QtSerialPort @executable_path/../Frameworks/QtSerialPort.framework/Versions/5/QtSerialPort $PROJECT.app/Contents/MacOS/${PROJECT}Tool
install_name_tool -change /usr/local/opt/qt@5/lib/QtNetwork.framework/Versions/5/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/5/QtNetwork $PROJECT.app/Contents/MacOS/${PROJECT}Tool
install_name_tool -change /usr/local/opt/qt@5/lib/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore $PROJECT.app/Contents/MacOS/${PROJECT}Tool

FAILED_LIBS_COUNT=`otool -L $PROJECT.app/Contents/Frameworks/*.dylib | grep /usr/local -c || true`
FAILED_APPS_COUNT=`otool -L $PROJECT.app/Contents/MacOS/* | grep /usr/local -c || true`

if [[ $FAILED_LIBS_COUNT -gt 0 ]]
then
    echo "Not all libraries use proper paths"
    exit 255

elif [[ $FAILED_APPS_COUNT -gt 0 ]]
then
   echo "Not all executables use proper paths"
   exit 255
fi

# Sign
if [ -n "$MAC_OS_SIGNING_KEY_ID" ]
then
    xattr -cr "$PROJECT.app"
    codesign --force --options=runtime -s "$MAC_OS_SIGNING_KEY_ID" --deep -v "$PROJECT.app"
    /usr/bin/ditto -c -k --keepParent "$PROJECT.app" "$PROJECT.zip"
    xcrun altool \
        --notarize-app \
        --primary-bundle-id "$MAC_OS_SIGNING_BUNDLE_ID" \
        --username "$MAC_OS_SIGNING_USERNAME" \
        --password "$MAC_OS_SIGNING_PASSWORD" \
        --asc-provider $MAC_OS_SIGNING_ASC_PROVIDER \
        --file "$PROJECT.zip"
fi

# build DMG
mkdir disk_image
mv $PROJECT.app disk_image
create-dmg \
    --volname "$PROJECT" \
    --volicon "../installer-assets/icons/${PROJECT}-installer.icns" \
    --background "../installer-assets/backgrounds/qFlipper_disk_background.png" \
    --window-pos 200 120 \
    --window-size 600 400 \
    --icon-size 100 \
    --icon "$PROJECT.app" 125 150 \
    --hide-extension "$PROJECT.app" \
    --app-drop-link 485 150 \
    "$PROJECT.dmg" \
    "disk_image/"

