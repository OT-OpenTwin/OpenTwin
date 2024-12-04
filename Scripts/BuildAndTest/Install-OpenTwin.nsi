/*

This example script installs a simple application for all users on a machine.

All-users installers should only write to HKLM, $ProgramFiles, $CommonFiles and the 
"All context" versions of $LocalAppData, $Templates, $SMPrograms etc.

It should not write to HKCU nor any folders in the users profile!
If the application requires writable template data in $AppData it 
must copy the required files from a shared location the 
first time a user launches the application.

*/

!include Mui2.nsh

!define NAME "OpenTwin_Frontend"
!define REGPATH_UNINSTSUBKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
Name "${NAME}"
OutFile "Install_${NAME}.exe"
Unicode True
!define MUI_PAGE_HEADER_TEXT "Welcome to OpenTwin"
BrandingText "(C) OpenTwin 2024"
RequestExecutionLevel user ; User rights on WinVista+ (when UAC is turned on)
InstallDir "$LOCALAPPDATA\$(^Name)"
InstallDirRegKey HKCU "${REGPATH_UNINSTSUBKEY}" "UninstallString"

!define OPENTWIN_APP_NAME "OpenTwin"
!define OPENTWIN_APP_ICON "$InstDir\icons\Application\OpenTwin.ico"

!include LogicLib.nsh
!include Integration.nsh

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\License.md"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN "$InstDir\OpenTwin.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch OpenTwin"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

!macro UninstallExisting exitcode uninstcommand
	Push `${uninstcommand}`
	Call UninstallExisting
	Pop ${exitcode}
!macroend

Function UninstallExisting
	Exch $1 ; uninstcommand
	Push $2 ; Uninstaller
	Push $3 ; Len
	StrCpy $3 ""
	StrCpy $2 $1 1
	StrCmp $2 '"' qloop sloop
sloop:
	StrCpy $2 $1 1 $3
	IntOp $3 $3 + 1
	StrCmp $2 "" +2
	StrCmp $2 ' ' 0 sloop
	IntOp $3 $3 - 1
	Goto run
qloop:
	StrCmp $3 "" 0 +2
	StrCpy $1 $1 "" 1 ; Remove initial quote
	IntOp $3 $3 + 1
	StrCpy $2 $1 1 $3
	StrCmp $2 "" +2
	StrCmp $2 '"' 0 qloop
run:
	StrCpy $2 $1 $3 ; Path to uninstaller
	StrCpy $1 161 ; ERROR_BAD_PATHNAME
	GetFullPathName $3 "$2\.." ; $InstDir
	IfFileExists "$2" 0 +4
	ExecWait '"$2" /S _?=$3' $1 ; This assumes the existing uninstaller is a NSIS uninstaller, other uninstallers don't support /S nor _?=
	IntCmp $1 0 "" +2 +2 ; Don't delete the installer if it was aborted
	Delete "$2" ; Delete the uninstaller
	RMDir "$3" ; Try to delete $InstDir
	RMDir "$3\.." ; (Optional) Try to delete the parent of $InstDir

	Pop $3
	Pop $2
	Exch $1 ; exitcode
FunctionEnd

Function .onInit
	SetShellVarContext current
	
	Sleep 1000
	
	ReadEnvStr $0 SYSTEMROOT
testRunning:
	nsExec::Exec 'CMD /C $0\System32\tasklist /NH /FI "IMAGENAME eq OpenTwin.exe" | $0\System32\find /I "OpenTwin.exe"'
	Pop $1
	${If} $1 == 0
		MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "Application is running. Please close all existing OpenTwin applications to continue installation." IDOK +2
		Abort
		GOTO testRunning
	${EndIf}		

	ReadRegStr $0 HKCU ${REGPATH_UNINSTSUBKEY} "UninstallString"
	${If} $0 != "" 
		!insertmacro UninstallExisting $0 $0
	${EndIf}
	
	ReadRegStr $0 HKCU ${REGPATH_UNINSTSUBKEY} "UninstallString"
	${If} $0 != "" 
		MessageBox MB_OK|MB_ICONSTOP "Failed to uninstall, installation failed" 
		Abort
	${EndIf}
FunctionEnd

Function un.onInit
  SetShellVarContext current
FunctionEnd

Section "Program files (Required)"
  SectionIn Ro

  SetOutPath $InstDir
  
  WriteUninstaller "$InstDir\Uninst.exe"
  WriteRegStr HKCU "${REGPATH_UNINSTSUBKEY}" "DisplayName" "OpenTwin"
  WriteRegStr HKCU "${REGPATH_UNINSTSUBKEY}" "DisplayIcon" "$InstDir\icons\Application\OpenTwin.ico"
  WriteRegStr HKCU "${REGPATH_UNINSTSUBKEY}" "UninstallString" '"$InstDir\Uninst.exe"'
  WriteRegDWORD HKCU "${REGPATH_UNINSTSUBKEY}" "NoModify" 1
  WriteRegDWORD HKCU "${REGPATH_UNINSTSUBKEY}" "NoRepair" 1
  
  File /r ..\..\Deployment_Frontend\*.*
  
  IfSilent "" +2 ; If the installer is silent then we run the executable in a last step
  ExecShell "" "$InstDir\OpenTwin.exe"
  
SectionEnd

Section "Start Menu shortcut"
	CreateShortcut "$SMPROGRAMS\${OPENTWIN_APP_NAME}.lnk" "$InstDir\OpenTwin.exe" "" "${OPENTWIN_APP_NAME}"
	CreateShortcut "$DESKTOP\${OPENTWIN_APP_NAME}.lnk" "$InstDir\OpenTwin.exe" "" "${OPENTWIN_APP_NAME}"
SectionEnd

Section -Uninstall

	${UNPINSHORTCUT} "$SMPROGRAMS\${OPENTWIN_APP_NAME}.lnk"
	Delete "$SMPROGRAMS\${OPENTWIN_APP_NAME}.lnk"
	${UNPINSHORTCUT} "$DESKTOP\${OPENTWIN_APP_NAME}.lnk"
	Delete "$DESKTOP\${OPENTWIN_APP_NAME}.lnk"
	
	RMDir /r "$InstDir"

	DeleteRegKey HKCU "${REGPATH_UNINSTSUBKEY}"
SectionEnd