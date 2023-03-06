@echo off
setlocal EnableDelayedExpansion

set ARCH_BITS=64

set MSVC_VERSION=2019
set "MSVC_DIR=%programfiles(x86)%\Microsoft Visual Studio\%MSVC_VERSION%"

rem Import build environment
for %%s in (Community Professional Enterprise) do (
    set "MSVC_VCVARS_PATH=%MSVC_DIR%\%%s\VC\Auxiliary\Build\vcvars%ARCH_BITS%.bat"
    if exist !MSVC_VCVARS_PATH! (
        goto foundmsvc
    )
)

echo Could not find MSVC && goto error

:foundmsvc
call "!MSVC_VCVARS_PATH!"

set QT_DIR=C:\Qt
set QT_VERSION=6.4.2
set QT_COMPILER=msvc2019_%ARCH_BITS%
set QT_BIN_DIR=%QT_DIR%\%QT_VERSION%\%QT_COMPILER%\bin

rem Download here https://cdn.flipperzero.one/STM32_DFU_USB_Driver.zip
set STM32_DRIVER_DIR="C:\STM32 Driver"

set QMAKE=%QT_BIN_DIR%\qmake.exe
set WINDEPLOYQT=%QT_BIN_DIR%\windeployqt.exe
set JOM=%QT_DIR%\Tools\QtCreator\bin\jom\jom.exe

set TARGET=qFlipper
set TARGET_CLI=%TARGET%-cli
set PROTO_TARGET=flipperproto
set DRIVER_TOOL=FlipperDriverTool

set PROJECT_DIR=%cd%
set BUILD_DIR=%PROJECT_DIR%\build
set QML_DIR=%PROJECT_DIR%\Application
set DIST_DIR=%BUILD_DIR%\%TARGET%
set PLUGINS_DIR=%DIST_DIR%\plugins
set DRIVER_TOOL_DIR=%PROJECT_DIR%\driver-tool

set OPENSSL_DIR=%QT_DIR%\Tools\OpenSSL\Win_x%ARCH_BITS%\bin
set VCREDIST_DIR=%QT_DIR%\vcredist

set NSIS="%programfiles(x86)%\NSIS\makensis.exe"

set VCREDIST2019_EXE=%VCREDIST_DIR%\vcredist_msvc%MSVC_VERSION%_x%ARCH_BITS%.exe
rem Visual C++ 2010 from Qt5 package is outdated and have exe sign from 2014.
rem It should be replaced with new version that have year 2021 signature, downloaded from Microsoft website
set VCREDIST2010_EXE=%VCREDIST_DIR%\vcredist_x%ARCH_BITS%.exe

rem if exist %BUILD_DIR% (rmdir /S /Q %BUILD_DIR%)

rem Build the application
mkdir %BUILD_DIR%
cd %BUILD_DIR%

%QMAKE% %PROJECT_DIR%\%TARGET%.pro -spec win32-msvc "CONFIG+=qtquickcompiler" &&^
%JOM% qmake_all && %JOM% && %JOM% install || goto error

rem Deploy the application
cd %DIST_DIR%

%WINDEPLOYQT% --release --no-compiler-runtime --dir %DIST_DIR% %PLUGINS_DIR%/%PROTO_TARGET%0.dll &&^
%WINDEPLOYQT% --release --no-compiler-runtime --qmldir %QML_DIR% %TARGET%.exe &&^
%WINDEPLOYQT% --release --no-compiler-runtime %TARGET_CLI%.exe || goto error

rem Copy OpenSSL binaries
copy /Y %OPENSSL_DIR%\*.dll .

rem Copy the driver
xcopy /Y /E /I %STM32_DRIVER_DIR% %DIST_DIR%\"STM32 Driver"

rem Copy Microsoft Visual C++ redistributable packages
copy /Y %VCREDIST2019_EXE% .
copy /Y %VCREDIST2010_EXE% .

if defined SIGNING_TOOL (
    rem Sign the executables
    call %SIGNING_TOOL% %DIST_DIR%\%TARGET%.exe || goto error
    call %SIGNING_TOOL% %DIST_DIR%\%TARGET_CLI%.exe || goto error
)

rem Make the zip archive as well
tar -a -cf %BUILD_DIR%\%TARGET%-%ARCH_BITS%bit.zip *

rem Make the installer
cd %PROJECT_DIR%
%NSIS% /DNAME=%TARGET% /DARCH_BITS=%ARCH_BITS% installer_windows.nsi || goto error

timeout /T 5 /NOBREAK > nul

if defined SIGNING_TOOL (
    rem Sign the installer
    call %SIGNING_TOOL% %BUILD_DIR%\%TARGET%Setup-%ARCH_BITS%bit.exe || goto error
)

echo The resulting installer is %BUILD_DIR%\%TARGET%Setup-%ARCH_BITS%bit.exe.
echo Finished.
exit 0

:error
echo There were errors during the build process!
exit 1
