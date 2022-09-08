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

#../configure \
#    -static \
#    -release \
#    -prefix "$QT_INSTALL_DIR" \
#    -skip "qtwebengine" \
#    -nomake "tests" \
#    -nomake "examples" \
#    -no-sql-mysql \
#    -no-sql-odbc \
#    -no-sql-psql \
#    -system-sqlite \
/opt/homebrew/Cellar/cmake/3.24.1/bin/cmake \
    '-DBUILD_qtwebengine=OFF' \
    '-DBUILD_SHARED_LIBS=OFF' \
    '-DCMAKE_INSTALL_PREFIX=/opt/homebrew/qt-6.3.1-static' \
    '-DQT_BUILD_TESTS=FALSE' \
    '-DQT_BUILD_EXAMPLES=FALSE' \
    '-DCMAKE_BUILD_TYPE=Release' \
    '-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64;' \
    '-DQT_BUILD_TESTS_BY_DEFAULT=OFF' \
    '-DINPUT_sql_mysql=no' \
    '-DINPUT_sql_odbc=no' \
    '-DINPUT_sql_psql=no' \
    '-DINPUT_system_sqlite=no' \
    '-G' 'Ninja' \
    '/Users/build/qt-everywhere-src-6.3.1'

cmake --build . --parallel
cmake --install .
