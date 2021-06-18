@echo off

set ARCH_BITS=64

set MSVC_VERSION=2019
set MSVC_DIR="C:\Program Files (x86)\Microsoft Visual Studio\"%MSVC_VERSION%

rem Import build environment
call %MSVC_DIR%\"Community\VC\Auxiliary\Build\"vcvars%ARCH_BITS%.bat

set QT_DIR=C:\Qt
set QT_VERSION=5.12.11
set QT_COMPILER=msvc2017_%ARCH_BITS%
set QT_BIN_DIR=%QT_DIR%\%QT_VERSION%\%QT_COMPILER%\bin

set QMAKE=%QT_BIN_DIR%\qmake.exe
set WINDEPLOYQT=%QT_BIN_DIR%\windeployqt.exe
set JOM=%QT_DIR%\Tools\QtCreator\bin\jom.exe

set TARGET=Flipartner

set PROJECT_DIR=%cd%
set BUILD_DIR=%PROJECT_DIR%\build
set QML_DIR=%PROJECT_DIR%\Application
set DIST_DIR=%BUILD_DIR%\%TARGET%

set OPENSSL_VERSION=1.1.1k
set OPENSSL_FILE_NAME=openssl-%OPENSSL_VERSION%-win%ARCH_BITS%.zip
set OPENSSL_URL=http://wiki.overbyte.eu/arch/%OPENSSL_FILE_NAME%

if exist %BUILD_DIR% (rmdir /S /Q %BUILD_DIR%)

mkdir %BUILD_DIR%
cd %BUILD_DIR%

rem Compile application
%QMAKE% %PROJECT_DIR%\%TARGET%.pro -spec win32-msvc "CONFIG+=qtquickcompiler"
%JOM% qmake_all
%JOM%

rem Deploy the application
mkdir %DIST_DIR%
move /Y %TARGET%.exe %DIST_DIR%
cd %DIST_DIR%

%WINDEPLOYQT% --qmldir %QML_DIR% %TARGET%.exe

rem Download OpenSSL binaries - temporary solution
curl  %OPENSSL_URL% -o %OPENSSL_FILE_NAME%
tar -xf %OPENSSL_FILE_NAME% *.dll
del /Q %OPENSSL_FILE_NAME%

tar -a -cf %BUILD_DIR%\%TARGET%.zip *

rmdir /S /Q %BUILD_DIR%\%TARGET%

cd %PROJECT_DIR%

cls
echo on
@echo The resulting file is %BUILD_DIR%\%TARGET%.zip.
@timeout 10 > NUL
