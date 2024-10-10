Var Upgr_Dialog_Txt
Var ServiceNameHandle

Var Upgr_Dialog
Var noMongoExists_RDB
Var MongoExists_RDB
Var Label
; First_MONGODB_Install_FLAG is defined im calling script


Function SetupMongoDBInstallation
	;Check if there is already a MongoDB service running
	nsExec::ExecToStack 'sc.exe query $UPGRADER_MONGODB_SERVICE_NAME'
	Pop $0
	;MessageBox MB_OK "Searched for active Service: $0"
	${If} $0 == 0 ;Service exists
		;MessageBox MB_OK "In the correct section"
		StrCpy $First_MONGODB_Install_FLAG 0
		StrCpy $Upgr_Dialog_Txt "A MongoDB service has been detected."
		Call Upgrader_On_Init ; Since there is no dialog, NSIS seems to skip the leave function
	${ElseIf} $0 == 1060 ;Service not found, confirm next
		StrCpy $First_MONGODB_Install_FLAG 1 ;The return code for server not found is 1060
		StrCpy $Upgr_Dialog_Txt "Failed to find a service running under the name MongoDB. If a MongoDB installation exists but is running under a different name, adjust the following information accordingly:"
		Call ConfirmNoMongoDBServer
	${Else}
		StrCpy $First_MONGODB_Install_FLAG -1 ;Service not found, confirm next
		StrCpy $Upgr_Dialog_Txt "Critical error when trying to find a MongoDB service. Exit code: $0"
		Call ConfirmNoMongoDBServer
	${EndIf}
FunctionEnd

Function OnSetupMongoDBInstallationLeave
	;MessageBox MB_OK "On setup leave: $First_MONGODB_Install_FLAG"
	
	${IfNot} $First_MONGODB_Install_FLAG == 0 ; Either no service of that name was found or search for service failed.
		Pop $ServiceNameHandle
		${NSD_GetText} $ServiceNameHandle $UPGRADER_MONGODB_SERVICE_NAME
	
		${NSD_GetState} $MongoExists_RDB $0
		${If} $0 == ${BST_CHECKED}		
			nsExec::ExecToStack 'sc.exe query $UPGRADER_MONGODB_SERVICE_NAME'
			Pop $0
			${If} $0 <> 0 ;Service could not be found
				${If} $0 == 1060
					MessageBox MB_OK "Failed to find a service of the name $UPGRADER_MONGODB_SERVICE_NAME"
				${Else}
					MessageBox MB_OK "Failed to request service $UPGRADER_MONGODB_SERVICE_NAME, Errorcode $0"
				${EndIf}
					Abort
			${Else} ;Service was found, thus not the first MongoDB installation
				StrCpy $First_MONGODB_Install_FLAG 0 ;Service was found, no fresh, first MongoDB installation
			${EndIf}
		${Else} ; User chose option that no MongoDB Service exists, thus new installtion
				StrCpy $First_MONGODB_Install_FLAG 1
		${EndIf}
	${EndIf}
	
	;We switch the sections off that are not needed if the installation is a first
	${If} $First_MONGODB_Install_FLAG == 1
		;Upgrade is not required, the new MongoDB version can be installed directly. Take out all sections that are needed for an Upgrade + Installation
		SectionSetText ${Sec_Upgr} "" ;Section of the upgrader
		SectionSetFlags ${Sec_Upgr} ${SF_RO}
		;MessageBox MB_OK "First installation."
		SectionSetText ${Sec_Mongo_Grp} "" 
		SectionSetFlags ${Sec_Mongo_Grp} ${SF_RO}
		StrCpy $MONGODB_SETUP_MSG "Choose what features of OpenTwin you want to install."
	${Else}
		Call Upgrader_On_Init
	${EndIf}
FunctionEnd

Function ConfirmNoMongoDBServer
	!insertmacro MUI_HEADER_TEXT "About an existing MongoDB installation" "$Upgr_Dialog_Txt" 
	nsDialogs::Create 1018
	Pop $Upgr_Dialog
	${If} $Upgr_Dialog == error
		;MessageBox MB_OK "Fail"
		Quit
	${EndIf}
	
	${NSD_CreateRadioButton} 0 0u 100% 12u "No MongoDB service exists"
	Pop $noMongoExists_RDB
	${NSD_Check} $noMongoExists_RDB
	${NSD_OnClick} $noMongoExists_RDB ToggleServiceSpecifics
	
	${NSD_CreateRadioButton} 0 20u 100% 12u "There exists a MongoDB service"
	Pop $MongoExists_RDB
	${NSD_OnClick} $MongoExists_RDB ToggleServiceSpecifics
	
	${NSD_CreateHLine} 0 45 100% 10u ""
		
	${NSD_CreateLabel} 0u 90u 100% 12u "Enter MongoDB service name:"
	Pop $Label
	${NSD_CreateText} 0u 102u 100% 12u "MongoDB"
	Pop $ServiceNameHandle
	
	${NSD_Edit_SetReadOnly} $ServiceNameHandle 1
	
	nsDialogs::Show		
FunctionEnd


;Function RequestMongoDBServerName
;	!insertmacro MUI_HEADER_TEXT "About an existing MongoDB installation" "$Upgr_Dialog_Txt" 
;	;MessageBox MB_OK "Entry"
;	nsDialogs::Create 1018
;	Pop $Upgr_Dialog
;	${If} $Upgr_Dialog == error
;		;MessageBox MB_OK "Fail"
;		Quit
;	${EndIf}
;	
;	${NSD_CreateLabel} 0u 30u 100% 12u "Enter MongoDB service name:"
;	Pop $Label
;	${NSD_CreateText} 0u 42u 100% 12u "MongoDB"
;	  
;  nsDialogs::Show
;FunctionEnd

Function ToggleServiceSpecifics
	${NSD_GetState} $noMongoExists_RDB $0
	${If} $0 == ${BST_CHECKED}
		;${NSD_Edit_SetReadOnly} $AdminPswHandle 1
		${NSD_Edit_SetReadOnly} $ServiceNameHandle 1
	${Else}
		;${NSD_Edit_SetReadOnly} $AdminPswHandle 0
		${NSD_Edit_SetReadOnly} $ServiceNameHandle 0
	${EndIf}
FunctionEnd