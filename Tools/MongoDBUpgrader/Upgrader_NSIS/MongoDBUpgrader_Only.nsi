!include "MUI2.nsh"

Name "MongoDB Upgrade"
Outfile "MongoDBUpgrade.exe"

;RequestExecutionLevel admin

!insertmacro MUI_LANGUAGE "English"

;!insertmacro MUI_PAGE_COMPONENTS

;Page custom InputPage
;Page instfiles


Var MONGODB_SERVICE_NAME
Var MONGODB_ADMIN_PASSWORD
Var MONGODB_INSTALL_PATH
Var MONGODB_SETUP_MSG
Var MONGODB_MAX_VERSION
Var Dialog
Var Label
Var Output

Function getUpgradeReq


FunctionEnd

Function InputPage
  nsDialogs::Create 1018
  Pop $Dialog

  ${NSD_CreateLabel} 0u 0u 100% 12u "Enter MongoDB admin Psw:"
  Pop $Label
  ${NSD_CreateText} 0u 12u 100% 12u "admin"
  Pop $MONGODB_ADMIN_PASSWORD
  
  ${NSD_CreateLabel} 0u 24u 100% 12u "Enter MongoDB service name:"
  Pop $Label
  ${NSD_CreateText} 0u 36u 100% 12u "MongoDB"
  Pop $MONGODB_SERVICE_NAME
  

  nsDialogs::Show
FunctionEnd
!define MUI_COMPONENTSPAGE_TEXT_TOP $MONGODB_SETUP_MSG
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES 

SectionGroup /e "MongoDB Setup"
Section "Upgrade data collection" Sec_Upgr
  Call InputPage
  nsExec::ExecToStack /OEM "$EXEDIR\MongoDBUpgrader.exe" "--Upgrade" "--Admin_Psw" "$MONGODB_ADMIN_PASSWORD"
  ;MessageBox MB_OK "Upgrade required: $1"
	MessageBox MB_OK "Upgrade"

SectionEnd

Section "Install MongoDB version 7.0" Sec_Inst
	ExecWait 'msiexec /l*v mdbinstall.log  /qb /i "$INSTDIR\MongoDB_Installer\mongodb-windows-x86_64-7.0.14-signed" INSTALLLOCATION="$MONGODB_INSTALL_PATH" SHOULD_INSTALL_COMPASS="0" ADDLOCAL="ServerService,Client"'
	;Now move the temporary cfg into the new installdirectory
SectionEnd
SectionGroupEnd


Function Upgrader_Failed
	MessageBox MB_OK "Failed to determine the current MongoDB version: $1"
	Quit
FunctionEnd

Function Upgrader_Upgrade_Required
	StrCpy $MONGODB_SETUP_MSG \
	"An installed MongoDB version $1 was detected. OpenTwin is now working with MongoDB version $MONGODB_MAX_VERSION and an upgrade is adviced. Be aware that an upgrade of the existing data is necessary and potentially cannot be reverted without help of the MongoDB support group."
	SectionSetFlags ${Sec_Upgr} ${SF_PSELECTED}
	SectionSetFlags ${Sec_Inst} ${SF_PSELECTED}
FunctionEnd

Function Upgrader_Upgrade_NOT_Required
	StrCpy $MONGODB_SETUP_MSG \
	"An installed MongoDB version was detected but no data in its database path was found. OpenTwin is now working with MongoDB version $MONGODB_MAX_VERSION and an upgrade is adviced." 
	SectionSetText ${Sec_Upgr} "" ;Upgrade is not required, the new MongoDB version can be installed directly.
	SectionSetFlags ${Sec_Upgr} ${SF_RO}
FunctionEnd

Function Upgrader_MongoDB_Up_To_Date
	;Nothing needs to be done.
	MessageBox MB_OK \
	"Detected MongoDB version $1 which is the most current that this installer supports."
	Quit
FunctionEnd

Function Upgrader_On_Init
  nsExec::ExecToStack /OEM "$EXEDIR\MongoDBUpgrader.exe --MaxVersion"
  Pop $0
  Pop $1
  StrCpy $MONGODB_MAX_VERSION $1
    MessageBox MB_OK "Returnvalue: $0 Returnmessage: $1"
	
  nsExec::ExecToStack /OEM "$EXEDIR\MongoDBUpgrader.exe --Check"
  Pop $0
  Pop $1
  MessageBox MB_OK "Returnvalue: $0 Returnmessage: $1"
 
  ;Exit codes: 
	;1: failed, the returned text holds the error message
	;2: upgrade required, the returned text holds found version
	;3: MongoDB found but no MongoDB data found
	;4: Mongo version up-to-date
	
  ${If} $0 == 1 
    Call Upgrader_Failed
  ${ElseIf} $0 == 2
	Call Upgrader_Upgrade_Required
  ${ElseIf} $0 == 3
	Call Upgrader_Upgrade_NOT_Required
  ${ElseIf} $0 == 4
    Call Upgrader_MongoDB_Up_To_Date
  ${Else} 
    MessageBox MB_OK "Failed to determine the current MongoDB version because of unknown return code: $0"
	Quit
  ${EndIf}
FunctionEnd


Function .onInit
	Call Upgrader_On_Init
	

FunctionEnd