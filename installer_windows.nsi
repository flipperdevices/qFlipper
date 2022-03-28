; qFlipper Windows Installer Build Script 
; requires NullSoft Installer 3.08 or later

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  Unicode true
  
  ;Compression algorithm used to compress files/data in the installer
  SetCompressor /solid /final lzma

  !define /ifndef NAME "qFlipper"
  !define /ifndef ARCH_BITS 64
  !define UNINSTALL_EXE "$INSTDIR\uninstall.exe"
  !define VCREDIST2019_EXE "$INSTDIR\vcredist_msvc2019_x${ARCH_BITS}.exe"
  !define VCREDIST2010_EXE "$INSTDIR\vcredist_x${ARCH_BITS}.exe"
  !define UNINSTALL_REG_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
  !define STM32_DRIVER_PATH "$INSTDIR\STM32 Driver"

  ; Include File Functions Header
  !include "FileFunc.nsh"

  ; Include macros to handle installations on x64 machines
  !include "x64.nsh"

  Name ${NAME}
  OutFile "build\${NAME}Setup-${ARCH_BITS}bit.exe"

  ; Get version tag from git. Will be used in titles
  !tempfile StdOut
  !echo "${StdOut}"
  !system '"git" describe --tags --abbrev=0 > "${StdOut}"'
  !define /file VERSION "${StdOut}"
  !delfile "${StdOut}"
  !undef StdOut

  ; Default installation Dir. On Windows it will be C:\Program Files\qFlipper
  InstallDir "$PROGRAMFILES64\${NAME}"

  ; Get installation folder from registry if available
  ; Variables not supported here
  InstallDirRegKey HKLM "Software\qFlipper" ""

  ; Installer Icon on left window corner and exe file
  !define MUI_ICON "installer-assets\icons\${NAME}-installer.ico"

  ; Enable scaling for high DPI screen
  ManifestDPIAware true

;--------------------------------
;Installer wizard pages

  ; Global window title 
  Caption "qFlipper ${VERSION} Setup"

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "installer-assets\backgrounds\windows_installer_header.bmp"
  !define MUI_HEADERIMAGE_UNBITMAP "installer-assets\backgrounds\windows_uninstaller_header.bmp"

  ; Welcome and Finish page settings
  !define MUI_WELCOMEPAGE_TITLE  "Welcome to qFlipper ${VERSION} Setup"
  !define MUI_WELCOMEPAGE_TEXT "qFlipepr is a desktop application for updating Flipper Zero firmware and databases, manage files on SD card, and repair corrupted device.$\r$\n$\r$\n$\r$\n$\r$\n$\r$\n$\r$\n$\r$\n$\r$\nCredits$\r$\nCode:   Georgii Surkov$\r$\nDesign: Valerie Aquamine, Dmitry Pavlov$\n$\r$\nOpen Source and Distrubted under GPL v3 License$\r$\nCopyright (C) 2022 Flipper Devices Inc."
  !define MUI_WELCOMEFINISHPAGE_BITMAP "installer-assets\backgrounds\windows_installer_welcome.bmp"
  !define MUI_UNWELCOMEFINISHPAGE_BITMAP "installer-assets\backgrounds\windows_uninstaller_welcome.bmp"
  !define MUI_PAGE_CUSTOMFUNCTION_SHOW showHiDpi ; HiDpi replace image hack for welcome page
  !insertmacro MUI_PAGE_WELCOME

  !insertmacro MUI_PAGE_DIRECTORY 
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_INSTFILES

  !define MUI_FINISHPAGE_TITLE "qFlipper ${VERSION} Setup Complete"
  !define MUI_FINISHPAGE_RUN "$INSTDIR\${NAME}.exe"
  !define MUI_FINISHPAGE_RUN_TEXT "Run qFlipper now"
  !define MUI_FINISHPAGE_LINK "More Info --> Flipper Zero Documentation"
  !define MUI_FINISHPAGE_LINK_LOCATION "https://docs.flipperzero.one"
  !define MUI_PAGE_CUSTOMFUNCTION_SHOW showHiDpi ; HiDpi replace image hack for finish page
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
; Initialize images files for HiDpi hack on every installer start

  Function .onInit
    InitPluginsDir
    File /oname=$PLUGINSDIR\windows_installer_welcome96.bmp installer-assets\backgrounds\windows_installer_welcome96.bmp
    File /oname=$PLUGINSDIR\windows_installer_welcome120.bmp installer-assets\backgrounds\windows_installer_welcome120.bmp
    File /oname=$PLUGINSDIR\windows_installer_welcome144.bmp installer-assets\backgrounds\windows_installer_welcome144.bmp
    File /oname=$PLUGINSDIR\windows_installer_welcome192.bmp installer-assets\backgrounds\windows_installer_welcome192.bmp

    File /oname=$PLUGINSDIR\windows_installer_header96.bmp installer-assets\backgrounds\windows_installer_header96.bmp
    File /oname=$PLUGINSDIR\windows_installer_header120.bmp installer-assets\backgrounds\windows_installer_header120.bmp
    File /oname=$PLUGINSDIR\windows_installer_header144.bmp installer-assets\backgrounds\windows_installer_header144.bmp
    File /oname=$PLUGINSDIR\windows_installer_header192.bmp installer-assets\backgrounds\windows_installer_header192.bmp
  FunctionEnd

; Function for dirty hijack image depends on DPI
  Function showHiDpi
    System::Call USER32::GetDpiForSystem()i.r0 
    ${If} $0 U<= 0 
        System::Call USER32::GetDC(i0)i.r1 
        System::Call GDI32::GetDeviceCaps(ir1,i88)i.r0 
        System::Call USER32::ReleaseDC(i0,ir1) 
    ${EndIf} 

    ${Unless} $0 == 120
    ${AndUnless} $0 == 144
    ${AndUnless} $0 == 192
        StrCpy $0 96
    ${EndIf}

    ${NSD_SetImage} $mui.WelcomePage.Image $PLUGINSDIR\windows_installer_welcome$0.bmp $mui.WelcomePage.Image.Bitmap
    ${NSD_SetImage} $mui.FinishPage.Image $PLUGINSDIR\windows_installer_welcome$0.bmp $mui.FinishPage.Image.Bitmap
    SetBrandingImage /IMGID=1046 "$PLUGINSDIR\windows_installer_header$0.bmp"
  FunctionEnd 

;--------------------------------
; Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

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

Section "USB DFU Driver" UsbDriverSection
	${DisableX64FSRedirection}
	nsExec::ExecToLog '"$SYSDIR\pnputil.exe" /add-driver "${STM32_DRIVER_PATH}\STM32Bootloader.inf" /install'
	${EnableX64FSRedirection}
SectionEnd

Section "Start menu entry" StartMenuSection
	CreateShortCut "$SMPROGRAMS\${NAME}.lnk" "$INSTDIR\${NAME}.exe"
SectionEnd

Section "Desktop shortcut" DesktopShortcutSection
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


;--------------------------------
; Descriptions
; A text hovers over a component on choosing components to install on MUI_PAGE_COMPONENTS
   
  ;Language strings
  LangString DESC_UsbDriverSection ${LANG_ENGLISH} "STM32 Bootloader Driver for Flipper DFU mode"
  LangString DESC_StartMenuSection ${LANG_ENGLISH} "Add qFlipper to Windows Start menu"
  LangString DESC_DesktopShortcutSection ${LANG_ENGLISH} "Create qFlipper shortcut on Desktop"
  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${UsbDriverSection} $(DESC_UsbDriverSection)
    !insertmacro MUI_DESCRIPTION_TEXT ${StartMenuSection} $(DESC_StartMenuSection)
    !insertmacro MUI_DESCRIPTION_TEXT ${DesktopShortcutSection} $(DESC_DesktopShortcutSection)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"
	Delete "$DESKTOP\${NAME}.lnk"
	Delete "$SMPROGRAMS\${NAME}.lnk"
	Delete "$INSTDIR\uninstall.exe"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
	RMDir /r $INSTDIR
SectionEnd
