!include "MUI2.nsh"

Name "MongoDB Upgrade"
Outfile "MongoDBUpgrade.exe"

Var UPGRADER_MONGODB_ADMIN_PASSWORD ; Needed for the execution of the upgrader
Var UPGRADER_MONGODB_SERVICE_NAME ; Needed for the execution of the upgrader
Var UPGRADER_MONGODB_INSTALL_PATH ; Needed for the execution of the installer

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


SectionGroup /e "MongoDB Setup"
Section "Upgrade data collection" Sec_Upgr
  Call InputPage
  nsExec::ExecToStack /OEM "$EXEDIR\MongoDBUpgrader.exe" "--Upgrade" "--Admin_Psw" "$UPGRADER_MONGODB_ADMIN_PASSWORD"
  ;MessageBox MB_OK "Upgrade required: $1"
	MessageBox MB_OK "Upgrade"

SectionEnd

Section "Install MongoDB version 7.0" Sec_Inst
	ExecWait 'msiexec /l*v mdbinstall.log  /qb /i "$INSTDIR\MongoDB_Installer\mongodb-windows-x86_64-7.0.14-signed" INSTALLLOCATION="$UPGRADER_MONGODB_INSTALL_PATH" SHOULD_INSTALL_COMPASS="0" ADDLOCAL="ServerService,Client"'
	;Now move the temporary cfg into the new installdirectory
SectionEnd
SectionGroupEnd