@echo off

set ARCH_BITS=64

set MSVC_VERSION=2019
set MSVC_DIR="%programfiles(x86)%\Microsoft Visual Studio\"%MSVC_VERSION%

rem Import build environment
call %MSVC_DIR%\"Community\VC\Auxiliary\Build\"vcvars%ARCH_BITS%.bat

set QT_DIR=C:\Qt
set QT_VERSION=5.15.2
set QT_COMPILER=msvc2019_%ARCH_BITS%
set QT_BIN_DIR=%QT_DIR%\%QT_VERSION%\%QT_COMPILER%\bin

set QMAKE=%QT_BIN_DIR%\qmake.exe
set WINDEPLOYQT=%QT_BIN_DIR%\windeployqt.exe
set JOM=%QT_DIR%\Tools\QtCreator\bin\jom.exe

set TARGET=qFlipper

set PROJECT_DIR=%cd%
set BUILD_DIR=%PROJECT_DIR%\build
set QML_DIR=%PROJECT_DIR%\Application
set DIST_DIR=%BUILD_DIR%\%TARGET%

set OPENSSL_VERSION=1.1.1k
set OPENSSL_FILE_NAME=openssl-%OPENSSL_VERSION%-win%ARCH_BITS%.zip
set OPENSSL_URL=http://wiki.overbyte.eu/arch/%OPENSSL_FILE_NAME%

set NSIS="%programfiles(x86)%\NSIS\makensis.exe"
set ZADIC_EXE="C:\bin-deps\zadic.exe"

if exist %BUILD_DIR% (rmdir /S /Q %BUILD_DIR%)

rem Build the application
mkdir %BUILD_DIR%
cd %BUILD_DIR%

%QMAKE% %PROJECT_DIR%\%TARGET%.pro -spec win32-msvc "CONFIG+=qtquickcompiler"
%JOM% qmake_all
%JOM%

rem Deploy the application
mkdir %DIST_DIR%
copy /Y %TARGET%.exe %DIST_DIR%
cd %DIST_DIR%

%WINDEPLOYQT% --release --qmldir %QML_DIR% %TARGET%.exe

rem Download OpenSSL binaries - temporary solution
curl  %OPENSSL_URL% -o %OPENSSL_FILE_NAME%
tar -xf %OPENSSL_FILE_NAME% *.dll
del /Q %OPENSSL_FILE_NAME%

rem Copy Zadic binary - also temporary solution?
copy /Y %ZADIC_EXE% .

rem Make the zip archive as well
tar -a -cf %BUILD_DIR%\%TARGET%-%ARCH_BITS%bit.zip *

rem Make the installer
cd %PROJECT_DIR%
%NSIS% /DNAME=%TARGET% /DARCH_BITS=%ARCH_BITS% installer_windows.nsi

echo The resulting installer is %BUILD_DIR%\%TARGET%Setup-%ARCH_BITS%bit.exe.
echo Finished.
