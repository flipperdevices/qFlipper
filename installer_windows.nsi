Unicode true

SetCompressor /solid /final lzma

!define /ifndef NAME "qFlipper"
!define /ifndef ARCH_BITS 64
!define UNINSTALL_EXE "$INSTDIR\uninstall.exe"
!define DRIVER_TOOL_EXE "$INSTDIR\FlipperDriverTool.exe"
!define VCREDIST2019_EXE "$INSTDIR\vcredist_msvc2019_x${ARCH_BITS}.exe"
!define VCREDIST2010_EXE "$INSTDIR\vcredist_x${ARCH_BITS}.exe"

Name ${NAME}
Icon "installer-assets\icons\${NAME}-installer.ico"

OutFile "build\${NAME}Setup-${ARCH_BITS}bit.exe"
InstallDir "$PROGRAMFILES64\${NAME}"

Page license
Page directory
Page components
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

LicenseData LICENSE

Section "-Main Application"
	IfFileExists "${UNINSTALL_EXE}" 0 +2
	ExecWait "${UNINSTALL_EXE} /S"
	
	SetOutPath $INSTDIR
	File /r "build\${NAME}\*"
	
	ExecWait "${VCREDIST2010_EXE} /passive /norestart"
	ExecWait "${VCREDIST2019_EXE} /install /passive /norestart"
	
	WriteUninstaller "${UNINSTALL_EXE}"
SectionEnd

Section "USB Driver"
	nsExec::ExecToLog '"${DRIVER_TOOL_EXE}" $HWNDPARENT'
SectionEnd

Section "Desktop shortcut"
	CreateShortCut "$DESKTOP\${NAME}.lnk" "$INSTDIR\${NAME}.exe"
SectionEnd

Section "-Cleanup"
	Delete ${VCREDIST2019_EXE}
	Delete ${VCREDIST2010_EXE}
	Delete ${DRIVER_TOOL_EXE}
SectionEnd

Section "Uninstall"
	Delete "$DESKTOP\${NAME}.lnk"
	Delete "$INSTDIR\uninstall.exe"
	RMDir /r $INSTDIR
SectionEnd
