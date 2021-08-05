Unicode true

SetCompressor /solid /final lzma

!define /ifndef NAME "qFlipper"
!define /ifndef ARCH_BITS 64
!define ZADIC_EXE "$INSTDIR\zadic.exe"
!define VCREDIST2019_EXE "$INSTDIR\vcredist_msvc2019_x${ARCH_BITS}.exe"
!define VCREDIST2010_EXE "$INSTDIR\vcredist_x${ARCH_BITS}.exe"

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
	
	ExecWait "${VCREDIST2010_EXE} /passive /norestart"
	ExecWait "${VCREDIST2019_EXE} /install /passive /norestart"
	ExecWait '${ZADIC_EXE} --vid 0x0483 --pid 0xdf11 --create "Flipper Zero"'
	
	Delete ${VCREDIST2019_EXE}
	Delete ${VCREDIST2010_EXE}
	Delete ${ZADIC_EXE}
	
	WriteUninstaller $INSTDIR\uninstall.exe
SectionEnd

Section "Uninstall"
	Delete $INSTDIR\uninstall.exe
	RMDir /r $INSTDIR
SectionEnd
