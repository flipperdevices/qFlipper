; qFlipper Windows Installer Build Script 
; requires NullSoft Installer 3.08 or later
; Reference http://kkmalar.org/WebApplication/qz-print-2.0.0-RC1/ant/windows/windows-packager.nsi.in


;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  Unicode true
  
  ;Compression algorithm used to compress files/data in the installer
  SetCompressor /solid /final lzma

  !define /ifndef NAME "qFlipper"
  !define /ifndef COMPANY "Flipper Devices Inc."
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

  ; Logic operators lib for calculating DPI
  !include 'LogicLib.nsh'

  ; Detect Windows Version lib
  !include 'WinVer.nsh'

  Name ${NAME}
  OutFile "build\${NAME}Setup-${ARCH_BITS}bit.exe"

  ; Get version tag from git. Will be used in titles
  !tempfile StdOut
  !echo "${StdOut}"
  !system '"git" describe --tags --abbrev=0 --exclude "*-rc*" > "${StdOut}"'
  !define /file VERSION "${StdOut}"
  !delfile "${StdOut}"
  !undef StdOut

  ; Default installation Dir. On Windows it will be C:\Program Files\qFlipper
  InstallDir "$PROGRAMFILES64\${NAME}"

  ; Installer/Uninstaller Icon
  !define MUI_ICON "installer-assets\icons\${NAME}-installer.ico"
  !define MUI_UNICON "installer-assets\icons\${NAME}-uninstaller.ico"

  ; Enable scaling for high DPI screen
  ManifestDPIAware true

  ; Sign the Uninstaller.exe file
  !uninstfinalize 'flipper_code_sign.bat "%1" wow64shit'

  ; Version Information displayer in Properties -> Details tab
  ; Required for antivirus databases
  VIProductVersion "${VERSION}.0" ; Only exact 4 numbers allowed x.x.x.x
  VIAddVersionKey "FileDescription" "qFlipper Windows Installer"
  VIAddVersionKey "FileVersion" "${VERSION}.0"
  VIAddVersionKey "ProductName" "qFlipper"  
  VIAddVersionKey "ProductVersion" "${VERSION}.0"
  VIAddVersionKey "CompanyName" "Flipper Devices Inc."
  VIAddVersionKey "LegalCopyright" "(C) Flipper Devices Inc."

;--------------------------------
;Installer wizard pages

  ; Global window title 
  Caption "qFlipper ${VERSION} Setup"

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "installer-assets\backgrounds\windows_installer\windows_installer_header216.bmp"
  !define MUI_HEADERIMAGE_UNBITMAP "installer-assets\backgrounds\windows_installer\windows_installer_header216.bmp"

  ; Welcome and Finish page settings
  !define MUI_WELCOMEPAGE_TITLE  "Welcome to qFlipper ${VERSION} Setup"
  !define MUI_WELCOMEPAGE_TEXT "qFlipper is a desktop application for updating Flipper Zero firmware and databases, manage files on SD card, and repair corrupted device.$\r$\n$\r$\n$\r$\n$\r$\n$\r$\n$\r$\n$\r$\n$\r$\nCredits$\r$\nCode:   Georgii Surkov$\r$\nDesign: Valerie Aquamine, Dmitry Pavlov$\n$\r$\nOpen Source and Distributed under GPL v3 License$\r$\nCopyright (C) 2022 Flipper Devices Inc."
  !define MUI_WELCOMEFINISHPAGE_BITMAP "installer-assets\backgrounds\windows_installer\windows_installer_welcome216.bmp"
  !define MUI_UNWELCOMEFINISHPAGE_BITMAP "installer-assets\backgrounds\windows_uninstaller\windows_uninstaller_welcome216.bmp"
  !define MUI_PAGE_CUSTOMFUNCTION_SHOW showHiDpi ; HiDpi replace image hack for welcome page
  !insertmacro MUI_PAGE_WELCOME

  !insertmacro MUI_PAGE_DIRECTORY 
  !insertmacro MUI_PAGE_COMPONENTS
  ;!define MUI_FINISHPAGE_NOAUTOCLOSE ; Debug
  !insertmacro MUI_PAGE_INSTFILES

  !define MUI_FINISHPAGE_TITLE "qFlipper ${VERSION} Setup Complete"
;  !define MUI_FINISHPAGE_RUN "$INSTDIR\${NAME}.exe"
;  !define MUI_FINISHPAGE_RUN_TEXT "Run qFlipper now"
  !define MUI_FINISHPAGE_LINK "More Info --> Flipper Zero Documentation"
  !define MUI_FINISHPAGE_LINK_LOCATION "https://docs.flipperzero.one"
  !define MUI_PAGE_CUSTOMFUNCTION_SHOW showHiDpi ; HiDpi replace image hack for finish page
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_COMPONENTS
  !define MUI_UNFINISHPAGE_NOAUTOCLOSE ; do not close uninstall log
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
; Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "-Main Application"

	RMDir /r $INSTDIR

    ; Use 64bit registry keys, not WOW6432Node
    SetRegView 64 

    ; Sets the context of shell folders to "All Users"
    SetShellVarContext all    
    ; Kills running qFlipper.exe processes
    DetailPrint "Looking for running qFlipper.exe..."
    nsExec::ExecToLog "wmic.exe PROCESS where $\"Name like 'qFlipper.exe'$\" CALL terminate"
    nsExec::ExecToLog "wmic.exe PROCESS where $\"Name like 'qFlipper.exe'$\" CALL terminate" ;Twice to avoid long time exiting
    SetShellVarContext current

	SetOutPath $INSTDIR

    ; Extract files
    SetOverwrite on
    File /r "build\${NAME}\*"
    

    ; Check if VC2010 installed and install it if not
    ReadRegStr $0 HKLM "SOFTWARE\WOW6432Node\Microsoft\VisualStudio\10.0\VC\VCRedist\x64" "Version"
    ${If} $0 == ""
      DetailPrint "Microsoft Visual C++ 2010 libs not found. Installing..."
      ExecWait "${VCREDIST2010_EXE} /q /norestart"
    ${Else}
      DetailPrint "Found Microsoft Visual C++ 2010 Version: $0"
    ${EndIf}
    
    ; Check if VC2019 installed and install it if not
    ReadRegStr $0 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Version"
    ${If} $0 == ""
      DetailPrint "Microsoft Visual C++ 2019 libs not found. Installing..."
      ExecWait "${VCREDIST2019_EXE} /install /quiet /norestart"
    ${Else}
      DetailPrint "Found Microsoft Visual C++ 2015-2019 Version: $0"
    ${EndIf}
    
    WriteUninstaller "${UNINSTALL_EXE}"

    WriteRegStr HKLM "Software\${NAME}" "" $INSTDIR ; Save real install path for next update
    WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayName" "${NAME} ${VERSION}"
    WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "Publisher" "${COMPANY}"
    WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "UninstallString" "$\"${UNINSTALL_EXE}$\""
    WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "QuietUninstallString" "$\"${UNINSTALL_EXE}$\" /S"
    WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayIcon" "$\"$INSTDIR\${NAME}.exe$\""
    WriteRegStr HKLM "${UNINSTALL_REG_PATH}" "DisplayVersion" "${VERSION}"
    WriteRegDWORD HKLM "${UNINSTALL_REG_PATH}" "NoModify" 1
    WriteRegDWORD HKLM "${UNINSTALL_REG_PATH}" "NoRepair" 1
SectionEnd

Section "USB DFU Driver" UsbDriverSection
  DetailPrint "Installing STM32 DFU Driver..."
  ${DisableX64FSRedirection}
	nsExec::ExecToLog '"$SYSDIR\pnputil.exe" /add-driver "${STM32_DRIVER_PATH}\STM32Bootloader.inf" /install'
SectionEnd

Section "Start menu entry" StartMenuSection
	CreateShortCut "$SMPROGRAMS\${NAME}.lnk" "$INSTDIR\${NAME}.exe"
SectionEnd

Section "Desktop shortcut" DesktopShortcutSection
	CreateShortCut "$DESKTOP\${NAME}.lnk" "$INSTDIR\${NAME}.exe"
SectionEnd

Section "-Cleanup"

    ; Use 64bit registry keys, not WOW6432Node
    SetRegView 64 

	Delete ${VCREDIST2019_EXE}
	Delete ${VCREDIST2010_EXE}
	;RMDir /r "${STM32_DRIVER_PATH}"

	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD HKLM "${UNINSTALL_REG_PATH}" "EstimatedSize" "$0"
SectionEnd


; Section to remove all Flipper Drivers, unchecked by default
Section /o "un.Remove Drivers" RemoveDriversSection
  DetailPrint "Removing drivers. This may take a while..."
  nsExec::ExecToLog '$SYSDIR\WindowsPowerShell\v1.0\powershell.exe -ExecutionPolicy RemoteSigned -File "${STM32_DRIVER_PATH}\delete_all_dfu_drivers.ps1"'
SectionEnd

;--------------------------------
;Uninstaller Section

Section "un.Uninstall qFlipper" UninstallqFlipperSection

  ; Use 64bit registry keys, not WOW6432Node
  SetRegView 64 

  ; Kills running qFlipper.exe processes
  DetailPrint "Looking for running qFlipper.exe..."
  nsExec::ExecToLog "wmic.exe PROCESS where $\"Name like 'qFlipper.exe'$\" CALL terminate"
  nsExec::ExecToLog "wmic.exe PROCESS where $\"Name like 'qFlipper.exe'$\" CALL terminate" ;Twice to avoid long time exiting

  Delete "$DESKTOP\${NAME}.lnk"
  Delete "$SMPROGRAMS\${NAME}.lnk"
  Delete "$INSTDIR\uninstall.exe"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
  DeleteRegKey HKLM "Software\${NAME}"
  RMDir /r $INSTDIR
SectionEnd


;--------------------------------
; Descriptions
; A text hovers over a component on choosing components to install on MUI_PAGE_COMPONENTS
   
  ;Language strings
  LangString DESC_UsbDriverSection ${LANG_ENGLISH} "STM32 Bootloader Driver for Flipper DFU mode"
  LangString DESC_StartMenuSection ${LANG_ENGLISH} "Add qFlipper to Windows Start menu"
  LangString DESC_DesktopShortcutSection ${LANG_ENGLISH} "Create qFlipper shortcut on Desktop"
  LangString DESC_RemoveDriversSection ${LANG_ENGLISH} "Remove all STM32 USB drivers from the system"
  LangString DESC_UninstallqFlipperSection ${LANG_ENGLISH} "Remove all STM32 USB drivers from the system"
  ;Assign language strings to install sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${UsbDriverSection} $(DESC_UsbDriverSection)
    !insertmacro MUI_DESCRIPTION_TEXT ${StartMenuSection} $(DESC_StartMenuSection)
    !insertmacro MUI_DESCRIPTION_TEXT ${DesktopShortcutSection} $(DESC_DesktopShortcutSection)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
  ;Assign language strings to UNinstall sections
  !insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${RemoveDriversSection} $(DESC_RemoveDriversSection)
  !insertmacro MUI_UNFUNCTION_DESCRIPTION_END


;-------------------------------
; Function runs on every installer exe start

  Function .onInit

    ; Abort if not Windows 10 and newer
    ${IfNot} ${AtLeastWin10}
      MessageBox MB_OK|MB_ICONSTOP "Can not install qFlipper. Windows 10 and newer required"
      Abort
    ${EndIf}

    ${If} ${RunningX64}
      ${DisableX64FSRedirection} ; Disable using SysWOW64 for 32-bit files
      SetRegView 64 ; Use 64bit registry keys, not WOW6432Node
    ${Else}
      MessageBox MB_OK|MB_ICONSTOP "Error: Can't install qFlipper on 32-bit Windows. Use 64-bit version of Windows"
      Abort ; Exit installer if 32 bit windows
     ${EndIf}  

    ; Get install dir from Registry
    ReadRegStr $R0 HKLM "Software\${NAME}" ""
    ; Set $INSTDIR only if registry value not empty
    ${If} $R0 != ""  
      StrCpy $INSTDIR $R0
    ${EndIf}

    ; Enable install log, need NSIS special build https://nsis.sourceforge.io/Special_Builds
    ;LogSet on ;  Debug


    ;-------------------------------
    ; Initialize images files for HiDpi hack on every installer start
    ; Refers to https://gist.github.com/sredna/c294cdf9014e03d8cd6f8bd4a39437ec
    ; http://forums.winamp.com/showthread.php?t=443754

    InitPluginsDir
    ; Installer Welcome images
    File /oname=$PLUGINSDIR\windows_installer_welcome96.bmp installer-assets\backgrounds\windows_installer\windows_installer_welcome96.bmp
    File /oname=$PLUGINSDIR\windows_installer_welcome120.bmp installer-assets\backgrounds\windows_installer\windows_installer_welcome120.bmp
    File /oname=$PLUGINSDIR\windows_installer_welcome144.bmp installer-assets\backgrounds\windows_installer\windows_installer_welcome144.bmp
    File /oname=$PLUGINSDIR\windows_installer_welcome168.bmp installer-assets\backgrounds\windows_installer\windows_installer_welcome168.bmp
    File /oname=$PLUGINSDIR\windows_installer_welcome192.bmp installer-assets\backgrounds\windows_installer\windows_installer_welcome192.bmp
    File /oname=$PLUGINSDIR\windows_installer_welcome216.bmp installer-assets\backgrounds\windows_installer\windows_installer_welcome216.bmp

    ; Installer Finish images 
    File /oname=$PLUGINSDIR\windows_installer_finish96.bmp installer-assets\backgrounds\windows_installer\windows_installer_finish96.bmp
    File /oname=$PLUGINSDIR\windows_installer_finish120.bmp installer-assets\backgrounds\windows_installer\windows_installer_finish120.bmp
    File /oname=$PLUGINSDIR\windows_installer_finish144.bmp installer-assets\backgrounds\windows_installer\windows_installer_finish144.bmp
    File /oname=$PLUGINSDIR\windows_installer_finish168.bmp installer-assets\backgrounds\windows_installer\windows_installer_finish168.bmp
    File /oname=$PLUGINSDIR\windows_installer_finish192.bmp installer-assets\backgrounds\windows_installer\windows_installer_finish192.bmp
    File /oname=$PLUGINSDIR\windows_installer_finish216.bmp installer-assets\backgrounds\windows_installer\windows_installer_finish216.bmp

    ; Installer/Uninstaller header
    File /oname=$PLUGINSDIR\windows_installer_header96.bmp installer-assets\backgrounds\windows_installer\windows_installer_header96.bmp
    File /oname=$PLUGINSDIR\windows_installer_header120.bmp installer-assets\backgrounds\windows_installer\windows_installer_header120.bmp
    File /oname=$PLUGINSDIR\windows_installer_header144.bmp installer-assets\backgrounds\windows_installer\windows_installer_header144.bmp
    File /oname=$PLUGINSDIR\windows_installer_header168.bmp installer-assets\backgrounds\windows_installer\windows_installer_header168.bmp
    File /oname=$PLUGINSDIR\windows_installer_header192.bmp installer-assets\backgrounds\windows_installer\windows_installer_header192.bmp
    File /oname=$PLUGINSDIR\windows_installer_header216.bmp installer-assets\backgrounds\windows_installer\windows_installer_header216.bmp
  FunctionEnd

;-------------------------------
; Function runs on every UNinstaller exe start
Function un.onInit
    ${If} ${RunningX64}
      ${DisableX64FSRedirection} ; Disable using SysWOW64 for 32-bit files
      SetRegView 64 ; Use 64bit registry keys, not WOW6432Node
    ${EndIf}
FunctionEnd

;-------------------------------
; Function for dirty hijack image depends on DPI
  Function showHiDpi
    System::Call USER32::GetDpiForSystem()i.r0 
    ${If} $0 U<= 0 
        System::Call USER32::GetDC(i0)i.r1 
        System::Call GDI32::GetDeviceCaps(ir1,i88)i.r0 
        System::Call USER32::ReleaseDC(i0,ir1) 
    ${EndIf} 
    
    ; If DPI greater than 216, do nothing and use default image and scaling
    ${If} $0 <= 216
      ${NSD_SetImage} $mui.WelcomePage.Image $PLUGINSDIR\windows_installer_welcome$0.bmp $mui.WelcomePage.Image.Bitmap
      ${NSD_SetImage} $mui.FinishPage.Image $PLUGINSDIR\windows_installer_finish$0.bmp $mui.FinishPage.Image.Bitmap
      SetBrandingImage /IMGID=1046 "$PLUGINSDIR\windows_installer_header$0.bmp"
    ${EndIf}
  FunctionEnd 

