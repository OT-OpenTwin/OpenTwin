;Outfile "MongoDBUpgrade.exe"
Var MONGODB_MAX_VERSION

Function Upgrader_On_Init
	;MessageBox MB_OK "Upgrader On Init: $First_MONGODB_Install_FLAG"
	${If} $First_MONGODB_Install_FLAG == 0
		;MessageBox MB_OK "Extracting MongoDBUpgrader Exes. LogPath: ${LOG_PATH}"
		CreateDirectory ${LOG_PATH}
		;MessageBox MB_OK "Extracting Exe "
		SetOutPath "${MONGODB_UPGRADER_INST_EXE_PATH}"
		File /r "${HELPER_FILES_PATH}\Upgrader_Exe\*.*"
				
		;MessageBox MB_OK "Extracting MongoServer"
		SetOutPath "${MONGODB_INST_SERVER_PATH}"
		File /r "..\..\..\..\ThirdParty\Installer_Tools\ThirdParty\shared\MongoDB_Server\*.*"
	
		;MessageBox MB_OK "Upgrader On Init, executes"
		nsExec::ExecToStack /OEM '"${MONGODB_UPGRADER_INST_EXE_PATH}\MongoDBUpgradeManager.exe" --MaxVersion --LogPath "${LOG_PATH}"'
		Pop $0
		Pop $1
		StrCpy $MONGODB_MAX_VERSION $1
		;MessageBox MB_OK "Return: $0 Message: $1"  
		
		;MessageBox MB_OK "Before execution"
		nsExec::ExecToStack /OEM '"${MONGODB_UPGRADER_INST_EXE_PATH}\MongoDBUpgradeManager.exe" --Check --LogPath "${LOG_PATH}"'
		Pop $0
		Pop $1
		;MessageBox MB_OK "Return: $0 Message: $1"  
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
			MessageBox MB_OK "Failed to determine the current MongoDB version due to unknown return code: $0"
			Quit
		${EndIf}
	${EndIf}
FunctionEnd

SectionGroup /e "MongoDB Setup" Sec_Mongo_Grp
Section "Upgrade data collection" Sec_Upgr
	${IfNot} ${SectionIsSelected} ${Sec_Upgr}
		Abort
	${EndIf}
	
	!insertmacro MUI_HEADER_TEXT "Upgrade of existing MongoDB data" "The upgrade requires a series of iterative steps. All steps are logged in ${LOG_PATH}\MongoDBUpgrader.log"
	MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION|MB_SETFOREGROUND "Preparing to upgrade the existing MongoDB stored data. Be aware that an upgrade my not be reversable without help of the official MongoDB support staff. Creating a back up of the existing data may be a good thing to do." IDOK ok IDCANCEL cancel
	cancel:
		;MessageBox MB_OK "Cancel"
		Abort
	ok:
		;MessageBox MB_OK "Execute Upgrade: ${MONGODB_UPGRADER_INST_EXE_PATH}\MongoDBUpgradeManager.exe --Upgrade --AdminPsw $UPGRADER_MONGODB_ADMIN_PASSWORD --ServiceName $UPGRADER_MONGODB_SERVICE_NAME"
		DetailPrint "Upgrading MongoDB data feature compatibility version ..."
		nsExec::ExecToStack /OEM '"${MONGODB_UPGRADER_INST_EXE_PATH}\MongoDBUpgradeManager.exe" --Upgrade --AdminPsw "$UPGRADER_MONGODB_ADMIN_PASSWORD" --ServiceName "$UPGRADER_MONGODB_SERVICE_NAME" --LogPath "${LOG_PATH}"'
SectionEnd

Section "Install MongoDB version 7.0" Sec_Inst
	${IfNot} ${SectionIsSelected} ${Sec_Inst}
		Abort
	${EndIf}

	;MessageBox MB_OK "Extracting MongoFiles"
	DetailPrint "Extracting MongoDB installer ..."
	
	SetOutPath "${MONGODB_INST_MSI_PATH}"
	File /r "..\..\..\..\ThirdParty\Installer_Tools\ThirdParty\shared\MongoDB_Installer\mongodb-windows-x86_64-7.0.14-signed.msi"

	
	AddSize 738000
	DetailPrint "Installing new MongoDB service ..."
	ExecWait 'msiexec /lxv mdbinstall.log /qb /i "${MONGODB_INST_MSI_PATH}\mongodb-windows-x86_64-7.0.14-signed.msi" INSTALLLOCATION="$UPGRADER_MONGODB_INSTALL_PATH" SHOULD_INSTALL_COMPASS="0" ADDLOCAL="ServerService"'		
	
	${If} $First_MONGODB_Install_FLAG == 0
		;MessageBox MB_OK "Adjusting Mongo Settings"
		DetailPrint "Adjusting service settings ..."
		nsExec::ExecToLog 'net stop "$UPGRADER_MONGODB_SERVICE_NAME"'
		nsExec::ExecToStack /OEM '"${MONGODB_UPGRADER_INST_EXE_PATH}\MongoDBUpgradeManager.exe" --SetMongoCfg --ServiceName "$UPGRADER_MONGODB_SERVICE_NAME" --LogPath "${LOG_PATH}"'
		nsExec::ExecToStack /OEM '"${MONGODB_UPGRADER_INST_EXE_PATH}\MongoDBUpgradeManager.exe" --VerifySetup --ServiceName "$UPGRADER_MONGODB_SERVICE_NAME" --LogPath "${LOG_PATH}"'
		nsExec::ExecToLog 'net start "$UPGRADER_MONGODB_SERVICE_NAME"'
	${Else}
		Call After_First_MongoDB_Install
	${EndIf}
SectionEnd

SectionGroupEnd
