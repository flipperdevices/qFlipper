#!/bin/bash

QT_VERSION="6.3.1";
QT_URL="https://download.qt.io/official_releases/qt/6.3/$QT_VERSION/single/qt-everywhere-src-$QT_VERSION.tar.xz"
QT_INSTALL_DIR="/opt/homebrew/qt-6.3.1-static"

rm -rf "qt-everywhere-src-$QT_VERSION";
wget "$QT_URL";
tar -xvf "qt-everywhere-src-$QT_VERSION.tar.zx";
cd "qt-everywhere-src-$QT_VERSION";
mkdir "build";
cd "build";

../configure \
    -static \
    -release \
    -prefix "$QT_INSTALL_DIR" \
    -skip "qtwebengine" \
    -nomake "tests" \
    -nomake "examples" \
    -no-sql-mysql \
    -no-sql-odbc \
    -no-sql-psql \
    -system-sqlite \

cmake --build . --parallel
cmake --install .
