Function DatabaseEntry
	${If} $First_MONGODB_Install_FLAG == 0
	${OrIfNot} ${SectionIsSelected} ${Sec_Inst}
		Abort
	${EndIf}

	!insertmacro MUI_HEADER_TEXT "MongoDB Directories" "MongoDB and its setup are required for OpenTwin to run. A location for the storage and log files are needed for the MongoDB installation"

	nsDialogs::Create 1018
	Pop $Dialog

	${If} $Dialog == error
		Abort
	${EndIf}

	${NSD_CreateLabel} 0 0 100% 30u "Please specify locations for the MongoDB installation, storage and log file dump. For the storage (databse) it is recommended to select directories on disks with a lot of space. These paths can later be changed in the mongod.cfg withing the MongoDB installation directory"
	#get DB location
	
	#custom installation path has been disabled due to a bug in the MongoDB installer
	${NSD_CreateLabel} 0 50 100% 10u "MongoDB Installation path:"
	${NSD_CreateText} 0 70 100% 12u ${DEFAULT_MONGODB_INSTALL_PATH}
		Pop $DirHandleMainInstall
		#variable assign for later use
		${NSD_GetText} $DirHandleMainInstall $UPGRADER_MONGODB_INSTALL_PATH
		SendMessage $DirHandleMainInstall ${EM_SETREADONLY} 1 0 #set DirHandleMainInstall to read only
		
	#uncomment these to re-enable the browse button
	#check 'Function SelectDBDirectory' also

	#${NSD_CreateBrowseButton} 350 68 65u 15u "Browse..."
	#	Pop $BrowseButton
	#${NSD_OnClick} $BrowseButton SelectMongoDBMainDirectory

	${NSD_CreateHLine} 0 110 100% 10u ""
	
	
	${NSD_CreateLabel} 0 125 100% 10u "MongoDB Database path:"
	${NSD_CreateText} 0 145 75% 12u ${DEFAULT_MONGODB_STORAGE_PATH}
		Pop $DirHandleDB
		${NSD_GetText} $DirHandleDB $MONGODB_DB_PATH
	${NSD_CreateBrowseButton} 350 143 65u 15u "Browse..."
		Pop $BrowseButton
	${NSD_OnClick} $BrowseButton SelectDBDirectory

	#get log location
	${NSD_CreateLabel} 0 180 100% 10u "MongoDB Log files path:"
	${NSD_CreateText} 0 200 75% 12u ${DEFAULT_MONGODB_LOG_PATH}
		Pop $DirHandleLog
		${NSD_GetText} $DirHandleLog $MONGODB_LOG_PATH
	${NSD_CreateBrowseButton} 350 198 65u 15u "Browse..."
		Pop $BrowseButton
	${NSD_OnClick} $BrowseButton SelectLogDirectory

	nsDialogs::Show
FunctionEnd

	/* #uncomment this to enable the functionality of the browse button	
		Function SelectMongoDBMainDirectory
			nsDialogs::SelectFolderDialog "Select a Directory" ""
			Pop $0 	#Get selected directory
			${If} $0 != error
				SendMessage $DirHandleMainInstall ${WM_SETTEXT} 0 "STR:$0" #assign selected directory to DirHandle
				StrCpy $UPGRADER_MONGODB_INSTALL_PATH $0
			${EndIf}
			
		FunctionEnd
	*/

Function SelectDBDirectory
	nsDialogs::SelectFolderDialog "Select a Directory" ""
	Pop $0 	#Get selected directory
	${If} $0 != error
		SendMessage $DirHandleDB ${WM_SETTEXT} 0 "STR:$0" #assign selected directory to DirHandle
		StrCpy $MONGODB_DB_PATH $0
	${EndIf}
	
FunctionEnd


Function SelectLogDirectory
	nsDialogs::SelectFolderDialog "Select a Directory" ""
	Pop $0 	#Get selected directory
	${If} $0 != error
		SendMessage $DirHandleLog ${WM_SETTEXT} 0 "STR:$0" #assign selected directory to DirHandle
		StrCpy $MONGODB_LOG_PATH $0
	${EndIf}
FunctionEnd

# DATABASE ADMIN PASSWORD (ACTIVE)
Function DatabasePassword
	
	${If} $First_MONGODB_Install_FLAG == 0
	${OrIfNot} ${SectionIsSelected} ${Sec_Inst}
		Abort
	${EndIf}
	
	!insertmacro MUI_HEADER_TEXT "MongoDB Administrator Password" "An administrator password needs to be specified for protecting the database."

	nsDialogs::Create 1018
	Pop $Dialog

	${If} $Dialog == error
		Abort
	${EndIf}
	
	#custom installation path has been disabled due to a bug in the MongoDB installer
	${NSD_CreateLabel} 0 50 100% 10u "Specify administrator password:"
	${NSD_CreatePassword} 0 70 100% 12u ${DEFAULT_MONGODB_ADMIN_PASSWORD}
	Pop $mongodb_admin_pwd1

	${NSD_CreateLabel} 0 120 100% 10u "Retype administrator password:"
	${NSD_CreatePassword} 0 140 100% 12u ${DEFAULT_MONGODB_ADMIN_PASSWORD}
	Pop $mongodb_admin_pwd2

	nsDialogs::Show
FunctionEnd
	
Function OnDatabasePasswordLeave

	${NSD_GetText} $mongodb_admin_pwd1 $0
	${NSD_GetText} $mongodb_admin_pwd2 $1
	
	StrCmp $0 $1 0 jump_to_if_not_equal
		StrCpy $UPGRADER_MONGODB_ADMIN_PASSWORD $0
		goto end
	jump_to_if_not_equal:
		MessageBox MB_ICONSTOP|MB_OK "The passwords do not match."
		Abort
	end:

FunctionEnd

	
/* DATABASE INFO PAGE (DISABLED)
	Function DatabaseInfo
		nsDialogs::Create 1018
		Pop $Dialog

		${If} $Dialog == error
			Abort
		${EndIf}

		${NSD_CreateLabel} 0 0 100% 10u "UPGRADER_MONGODB_INSTALL_PATH:"
			${NSD_CreateLabel} 0 25 100% 30 $UPGRADER_MONGODB_INSTALL_PATH

		${NSD_CreateLabel} 0 100 100% 10u "MONGODB_DB_PATH:"
			${NSD_CreateLabel} 0 125 100% 30 $MONGODB_DB_PATH

		${NSD_CreateLabel} 0 175 100% 10u "MONGODB_LOG_PATH:"
			${NSD_CreateLabel} 0 200 100% 30 $MONGODB_LOG_PATH
		nsDialogs::Show
	FunctionEnd
*/
Function After_First_MongoDB_Install
	Sleep 5000
	;MessageBox MB_OK " Path $UPGRADER_MONGODB_INSTALL_PATH"
	SetOutPath "$UPGRADER_MONGODB_INSTALL_PATH\bin"

	File /r "..\..\..\..\ThirdParty\Installer_Tools\ThirdParty\shared\MongoDB_Installer\mongosh.exe"
	
	nsExec::ExecToLog 'net stop "MongoDB"'	

	##########################################
	# call for python scripts via $INSTDIR
	##########################################
	
	DetailPrint "Running scripts..."

	# update the mongodB config file without authentication
	;ExecWait '"$INSTDIR\ConfigMongoDBNoAuth.exe" "$UPGRADER_MONGODB_INSTALL_PATH\bin\mongod.cfg" "$MONGODB_DB_PATH" "$MONGODB_LOG_PATH" $NetworkModeSelection "$UPGRADER_MONGODB_ADMIN_PASSWORD" "$INSTDIR\Tools\javascript\db_admin.js"'
	nsExec::Exec '"$INSTDIR\ConfigMongoDBNoAuth.exe" "$UPGRADER_MONGODB_INSTALL_PATH\bin\mongod.cfg" "$MONGODB_DB_PATH" "$MONGODB_LOG_PATH" $NetworkModeSelection $MONGODB_CUSTOM_PORT "$UPGRADER_MONGODB_ADMIN_PASSWORD" "$INSTDIR\Tools\javascript\db_admin.js"'

	SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000
	
	#set directory permissions for the mongoDB service
	;ExecWait '"$INSTDIR\SetPermissions.exe" "$MONGODB_DB_PATH" "$MONGODB_LOG_PATH"'
	nsExec::Exec '"$INSTDIR\SetPermissions.exe" "$MONGODB_DB_PATH" "$MONGODB_LOG_PATH"'

	# restarting mongoDB service
	# Should not be necessary. First we do the configuration: Set permissions for the data and log folder, then we adjust the config with costumised port, ip and costumised data and log paths. No authorisation and TLS off
	nsExec::ExecToLog 'net start "MongoDB"'
	Sleep 5000

	# 'net' command waits for the service to be stopped/started automatically
	# no additional checks needed

	# call for js script to paste admin user creation
	ExpandEnvStrings $0 %COMSPEC%
		;ExecWait '"$0" /c "START /WAIT /MIN cmd.exe /c " "$UPGRADER_MONGODB_INSTALL_PATH\bin\mongosh.exe" --host $NetworkModeSelection --port $MONGODB_CUSTOM_PORT < "$INSTDIR\Tools\javascript\db_admin.js_tmp" " "'
		nsExec::Exec '"$0" /c "START /WAIT /MIN cmd.exe /c " "$UPGRADER_MONGODB_INSTALL_PATH\bin\mongosh.exe" --host $NetworkModeSelection --port $MONGODB_CUSTOM_PORT < "$INSTDIR\Tools\javascript\db_admin.js_tmp" " "'

	Delete "$INSTDIR\Tools\javascript\db_admin.js_tmp"
	
	nsExec::ExecToLog 'net stop "MongoDB"'	

	# mongoDB_storage_script_wauth.py
	${If} $PublicIpSet <> 0
		;ExecWait '"$INSTDIR\ConfigMongoDBWithAuth.exe" "$UPGRADER_MONGODB_INSTALL_PATH\bin\mongod.cfg" "$PUBLIC_CERT_PATH\certificateKeyFile.pem" "$MONGODB_CUSTOM_PORT"'
		nsExec::Exec '"$INSTDIR\ConfigMongoDBWithAuth.exe" "$UPGRADER_MONGODB_INSTALL_PATH\bin\mongod.cfg" "$PUBLIC_CERT_PATH\certificateKeyFile.pem"'
		ExpandEnvStrings $0 %COMSPEC%
			;ExecWait '"$0" /c "START /WAIT /MIN cmd.exe /c "certutil -addstore root "$PUBLIC_CERT_PATH\ca.pem"""" '	
			nsExec::Exec '"$0" /c "START /WAIT /MIN cmd.exe /c "certutil -addstore root "$PUBLIC_CERT_PATH\ca.pem"""" '	
	${Else}
		;ExecWait '"$INSTDIR\ConfigMongoDBWithAuth.exe" "$UPGRADER_MONGODB_INSTALL_PATH\bin\mongod.cfg" "$DEFAULT_CERT_PATH\certificateKeyFile.pem" "$MONGODB_CUSTOM_PORT"'
		nsExec::Exec '"$INSTDIR\ConfigMongoDBWithAuth.exe" "$UPGRADER_MONGODB_INSTALL_PATH\bin\mongod.cfg" "$DEFAULT_CERT_PATH\certificateKeyFile.pem"'
		ExpandEnvStrings $0 %COMSPEC%
			;ExecWait '"$0" /c "START /WAIT /MIN cmd.exe /c "certutil -addstore root "$DEFAULT_CERT_PATH\ca.pem"""" '
			nsExec::Exec '"$0" /c "START /WAIT /MIN cmd.exe /c "certutil -addstore root "$DEFAULT_CERT_PATH\ca.pem"""" '
	${EndIf}
		
	DetailPrint "Restart services..."
	nsExec::ExecToLog 'net start "MongoDB"'	
	
	#DetailPrint "Wait..."
	#Sleep 10000
FunctionEnd
