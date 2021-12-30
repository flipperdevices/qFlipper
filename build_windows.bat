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
set DRIVER_TOOL=FlipperDriverTool

set PROJECT_DIR=%cd%
set BUILD_DIR=%PROJECT_DIR%\build
set QML_DIR=%PROJECT_DIR%\Application
set DIST_DIR=%BUILD_DIR%\%TARGET%
set DRIVER_TOOL_DIR=%PROJECT_DIR%\driver-tool

set OPENSSL_DIR=%QT_DIR%\Tools\OpenSSL\Win_x%ARCH_BITS%\bin
set VCREDIST_DIR=%QT_DIR%\vcredist

set NSIS="%programfiles(x86)%\NSIS\makensis.exe"

set VCREDIST2019_EXE=%VCREDIST_DIR%\vcredist_msvc%MSVC_VERSION%_x%ARCH_BITS%.exe
set VCREDIST2010_EXE=%VCREDIST_DIR%\vcredist_x%ARCH_BITS%.exe

if exist %BUILD_DIR% (rmdir /S /Q %BUILD_DIR%)

rem Build the application
mkdir %BUILD_DIR%
cd %BUILD_DIR%

%QMAKE% %PROJECT_DIR%\%TARGET%.pro -spec win32-msvc "CONFIG+=qtquickcompiler" || goto error
%JOM% qmake_all || goto error
%JOM% || goto error

rem Deploy the application
mkdir %DIST_DIR%
copy /Y %TARGET%.exe %DIST_DIR%
cd %DIST_DIR%

%WINDEPLOYQT% --release --no-compiler-runtime --qmldir %QML_DIR% %TARGET%.exe || goto error

rem Build the driver tool
msbuild %DRIVER_TOOL_DIR%\%DRIVER_TOOL%.sln /p:Configuration=Release /p:Platform=x%ARCH_BITS% || goto error

rem Copy the built driver tool
copy /Y %DRIVER_TOOL_DIR%\x%ARCH_BITS%\Release\%DRIVER_TOOL%.exe .

rem Copy OpenSSL binaries
copy /Y %OPENSSL_DIR%\*.dll .

rem Copy Microsoft Visual C++ redistributable packages
copy /Y %VCREDIST2019_EXE% .
copy /Y %VCREDIST2010_EXE% .

if defined SIGNING_TOOL (
	rem Sign the executables
	call %SIGNING_TOOL% %DIST_DIR%\%TARGET%.exe || goto error
	call %SIGNING_TOOL% %DIST_DIR%\%DRIVER_TOOL%.exe || goto error
)

rem Make the zip archive as well
tar -a -cf %BUILD_DIR%\%TARGET%-%ARCH_BITS%bit.zip *

rem Make the installer
cd %PROJECT_DIR%
%NSIS% /DNAME=%TARGET% /DARCH_BITS=%ARCH_BITS% installer_windows.nsi || goto error

timeout /T 5 /NOBREAK

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
