Unicode true

SetCompressor /solid /final lzma

!define /ifndef NAME "qFlipper"
!define /ifndef ARCH_BITS 64
!define UNINSTALL_EXE "$INSTDIR\uninstall.exe"
!define VCREDIST2019_EXE "$INSTDIR\vcredist_msvc2019_x${ARCH_BITS}.exe"
!define VCREDIST2010_EXE "$INSTDIR\vcredist_x${ARCH_BITS}.exe"
!define UNINSTALL_REG_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
!define STM32_DRIVER_PATH "$INSTDIR\STM32 Driver"

!include "FileFunc.nsh"
!include "x64.nsh"

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

	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayName" "${NAME}"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "UninstallString" "$\"${UNINSTALL_EXE}$\""
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "QuietUninstallString" "$\"${UNINSTALL_EXE}$\" /S"
	WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayIcon" "$\"$INSTDIR\${NAME}.exe$\""
	WriteRegDWORD HKLM "${UNINSTALL_REG_PATH}" "NoModify" 1
	WriteRegDWORD HKLM "${UNINSTALL_REG_PATH}" "NoRepair" 1
SectionEnd

Section "USB Driver"
	${DisableX64FSRedirection}
	nsExec::ExecToLog '"$SYSDIR\pnputil.exe" /add-driver "${STM32_DRIVER_PATH}\STM32Bootloader.inf" /install'
	${EnableX64FSRedirection}
SectionEnd

Section "Start menu entry"
	CreateShortCut "$SMPROGRAMS\${NAME}.lnk" "$INSTDIR\${NAME}.exe"
SectionEnd

Section "Desktop shortcut"
	CreateShortCut "$DESKTOP\${NAME}.lnk" "$INSTDIR\${NAME}.exe"
SectionEnd

Section "-Cleanup"
	Delete ${VCREDIST2019_EXE}
	Delete ${VCREDIST2010_EXE}
	RMDir /r "${STM32_DRIVER_PATH}"

	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD HKLM "${UNINSTALL_REG_PATH}" "EstimatedSize" "$0"
SectionEnd

Section "Uninstall"
	Delete "$DESKTOP\${NAME}.lnk"
	Delete "$SMPROGRAMS\${NAME}.lnk"
	Delete "$INSTDIR\uninstall.exe"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
	RMDir /r $INSTDIR
SectionEnd
