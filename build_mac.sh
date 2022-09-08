#!/bin/bash

set -exuo pipefail;

PROJECT="qFlipper";
BUILD_DIRECTORY="build_mac";

if [ -d ".git" ]; then
    git submodule update --init;
fi

if [[ "$(uname -s)" != "Darwin" ]]; then
    echo "This script needs to be runned under MacOS";
    exit 1;
fi

if [[ "$(uname -m)" == "arm64" ]]; then
    eval "$(/opt/homebrew/bin/brew shellenv)";
    PATH="/opt/homebrew/qt-6.3.1-static/bin:$PATH";
else
    eval "$(/usr/local/Homebrew/bin/brew shellenv)";
fi

rm -rf "$BUILD_DIRECTORY";
mkdir "$BUILD_DIRECTORY";

cd "$BUILD_DIRECTORY";

qmake \
    -spec macx-clang \
    CONFIG+=release \
    -o Makefile \
    ../$PROJECT.pro \
    QMAKE_APPLE_DEVICE_ARCHS="x86_64 arm64";

make qmake_all;
make "-j$(sysctl -n hw.ncpu)" > /dev/null 2>&1;
make install;

# bundle libusb
mkdir -p "$PROJECT.app/Contents/Frameworks";
cp "$(brew --prefix libusb)/lib/libusb-1.0.0.dylib" "$PROJECT.app/Contents/Frameworks";
install_name_tool \
    -change "$(brew --prefix libusb)/lib/libusb-1.0.0.dylib" \
    "@loader_path/libusb-1.0.0.dylib" \
    "$PROJECT.app/Contents/Frameworks/libusb-1.0.0.dylib";
install_name_tool \
    -change "$(brew --prefix libusb)/lib/libusb-1.0.0.dylib" \
    "@loader_path/../Frameworks/libusb-1.0.0.dylib" \
    "$PROJECT.app/Contents/MacOS/qFlipper";
install_name_tool \
    -change "$(brew --prefix libusb)/lib/libusb-1.0.0.dylib" \
    "@loader_path/../Frameworks/libusb-1.0.0.dylib" \
    "$PROJECT.app/Contents/MacOS/qFlipper-cli";

# Sign
if [ -n "${MAC_OS_SIGNING_KEY_ID:-""}" ]; then
    security default-keychain -s "$MAC_OS_KEYCHAIN_NAME";
    security unlock-keychain -p "$MAC_OS_KEYCHAIN_PASSWORD" "$MAC_OS_KEYCHAIN_NAME";
    xattr -cr "$PROJECT.app";
    codesign --force --options=runtime -s "$MAC_OS_SIGNING_KEY_ID" --deep -v "$PROJECT.app";
    /usr/bin/ditto -c -k --keepParent "$PROJECT.app" "$PROJECT.zip";
    xcrun altool \
        --notarize-app \
        --primary-bundle-id "$MAC_OS_SIGNING_BUNDLE_ID" \
        --username "$MAC_OS_SIGNING_USERNAME" \
        --password "$MAC_OS_SIGNING_PASSWORD" \
        --asc-provider "$MAC_OS_SIGNING_ASC_PROVIDER" \
        --file "$PROJECT.zip";
fi

# build DMG
mkdir disk_image;
mv "$PROJECT.app" "disk_image/";
cp "../installer-assets/macos/DS_Store" "disk_image/.DS_Store";
cp "../installer-assets/macos/VolumeIcon.icns" "disk_image/.VolumeIcon.icns";
cp -r "../installer-assets/macos/background" "disk_image/.background";
../scripts/create-dmg/create-dmg \
    --volname "$PROJECT-$(git describe --tags --abbrev=0)" \
    --skip-jenkins \
    --app-drop-link 485 150 \
    "$PROJECT.dmg" \
    "disk_image/";
