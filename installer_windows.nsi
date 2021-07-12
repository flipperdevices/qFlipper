Unicode true

SetCompressor /solid /final lzma

!define /ifndef NAME "qFlipper"
!define /ifndef ARCH_BITS 64
!define ZADIC_EXE "$INSTDIR\zadic.exe"

Name ${NAME}

OutFile "build\${NAME}Setup-${ARCH_BITS}bit.exe"
InstallDir "$PROGRAMFILES64\${NAME}"

Page license
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

LicenseData LICENSE

Section "-Main Application"
	SetOutPath $INSTDIR
	File /r "build\${NAME}\*"
	ExecWait '${ZADIC_EXE} --vid 0x0483 --pid 0xdf11 --create "Flipper Zero"'
	Delete ${ZADIC_EXE}
	WriteUninstaller $INSTDIR\uninstall.exe
SectionEnd

Section "Uninstall"
	Delete $INSTDIR\uninstall.exe
	RMDir /r $INSTDIR
SectionEnd
