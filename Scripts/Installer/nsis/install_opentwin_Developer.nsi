/*

	OpenTwin Developer Installation Script
	This script installs OpenTwin and all its dependencies and runs external 
	python scripts to set those up
	In addition to those actions, it installs the entire toolchain for an OpenTwin developer
	installation as well as cloning the public OpenTwin repository 

*/

!include "MUI.nsh"
!include LogicLib.nsh
!include nsDialogs.nsh
!include FileFunc.nsh
!include x64.nsh

#=================================================================
#							VARIABLES
#=================================================================
	Var Dialog
	#network mode page variables
	Var RadioButtonPublic
	Var public_ip_field
	Var localhost_radio_btn
	Var localhost_radio_btn_state
	Var RadioButtonPublic_state
	Var public_ip_field_content
	Var standardPortValuesRadioBtn
	Var standardPortValuesRadioBtnState
	Var customPortValuesRadioBtn
	Var customPortValuesRadioBtnState
	Var NetworkModeSelection

	Var ROOTDIR
	Var PublicCertPageChecker
	Var PublicIpSet
	Var DirHandleCert
	Var PUBLIC_CERT_PATH
	Var STANDARD_CERT_PATH

	#port variables
	Var auth_port
	Var auth_port_content

	Var gss_port
	Var gss_port_content

	Var lss_port
	Var lss_port_content

	Var gds_port
	Var gds_port_content

	Var lds_port
	Var lds_port_content

	Var MongoDBCustomPortField
	Var MongoDBCustomPortField_content
	Var MONGODB_CUSTOM_PORT

	Var PortReturnChecker

	Var auth_entry
	Var gss_entry
	Var lss_entry
	Var gds_entry
	Var lds_entry

	#Database (MongoDB) variables
	Var DirHandleMainInstall
	Var DirHandleDB
	Var DirHandleLog
	Var BrowseButton
	Var MONGODB_INSTALL_PATH
	Var MONGODB_DB_PATH
	Var MONGODB_LOG_PATH
	Var UNINSTALL_MONGODB_FLAG

	Var OPEN_TWIN_SERVICES_ADDRESS

	Var GITHUB_DESKTOP_DEPLOYMENT_INSTALL_LOCATION

	Var DevRootDirRequest
	Var DEV_ROOT
	Var OPENTWIN_DEV_ROOT
	Var OPENTWIN_THIRDPARTY_ROOT
	
	Var GitInstallPath
	Var TempToolChain

#=================================================================
#						END OF VARIABLES
#=================================================================


#=================================================================
#						GLOBAL DEFINES
#=================================================================

	!define PRODUCT_NAME "OpenTwin"
	BrandingText "[DEV] OpenTwin Simulation Platform"
	!define REGPATH_UNINSTSUBKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

	!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\OpenTwin.exe"

	#during installation icons
	!define MUI_WELCOMEFINISHPAGE_BITMAP "..\Graphics\Wizard\openTwin_headerImage_01.bmp"
	!define MUI_ICON_PATH '"..\Graphics\Icons\openTwin_icon_48x48.ico"'
	!define MUI_UNICON_PATH '"..\Graphics\Icons\opentwin_uninstall_icon_48x48.ico"'

	#post installation icons
	!define OPENTWIN_APP_ICON '"$INSTDIR\icons\Application\OpenTwin.ico"'
	!define OPENTWIN_UNAPP_ICON '"$INSTDIR\icons\Application\opentwin_uninstall_icon_48x48.ico"'

	!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
	!define PRODUCT_UNINST_ROOT_KEY "HKLM"
	!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

	!define CREATE_CERTIFICATE_BATCH '"$INSTDIR\Certificates\CreateServerCertificate_custom.cmd"'

	!define DEFAULT_MONGODB_STORAGE_PATH '"$DEV_ROOT\OpenTwin\Deployment\DataStorage\data"'
	!define DEFAULT_MONGODB_LOG_PATH '"$DEV_ROOT\OpenTwin\Deployment\DataStorage\log"'
	!define DEFAULT_MONGODB_INSTALL_PATH '"$PROGRAMFILES64\MongoDB\Server\4.4"'
	!define DEFAULT_MONGODB_COMPASS_PATH '"$PROGRAMFILES64\MongoDB Compass"'
	!define MONGODB_DELETION_PATH '"$PROGRAMFILES64\MongoDB"'

	!define OPENTWIN_REPO_GITADDRESS "git@github.com:OT-OpenTwin/OpenTwin.git"
	!define THIRDPARTY_REPO_GITADDRESS "git@github.com:OT-OpenTwin/ThirdParty.git"

	!define TEMP_TOOLCHAIN_DIR '$DEV_ROOT\_temp'

	!define DEV_ROOT_DIR '"$ROOTDIR\OT"'

	!define LICENSE_FILE_PATH '"..\..\..\LICENSE.md"'

RequestExecutionLevel admin

#from front end installer
!macro EnsureAdminRights
  UserInfo::GetAccountType
  Pop $0
  ${If} $0 != "admin" ; Require admin rights on WinNT4+
    MessageBox MB_IconStop "Administrator rights required!"
    SetErrorLevel 740 ; ERROR_ELEVATION_REQUIRED
    Quit
  ${EndIf}
!macroend

# ===================================================================
# ===================================================================
#from front end installer
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
# ===================================================================
# ===================================================================

#.onInit function to initialize any values that need initializing at the beginning of the script
Function .onInit
	#from front end installer
	!insertmacro EnsureAdminRights
	
	ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString"
		${If} $0 != ""
		${AndIf} ${Cmd} `MessageBox MB_YESNO|MB_ICONQUESTION "Uninstall previous version?" /SD IDYES IDYES`
			!insertmacro UninstallExisting $0 $0
				${If} $0 <> 0
					MessageBox MB_YESNO|MB_ICONSTOP "Failed to uninstall, continue anyway?" /SD IDYES IDYES +2
						Abort
				${EndIf}
		${EndIf}

	#check for 64-bit system
	${If} ${RunningX64}
        SetRegView 64
    ${EndIf}	
	ReadRegStr $1 HKLM "SOFTWARE\GitForWindows" "InstallPath"
		${If} $1 != ""
			Goto +5
		${Else}
			MessageBox MB_ICONSTOP|MB_OK "Git for Windows does not seem to be installed. This installer requires a fully set up Git installation before proceeding!"
			Abort
		${EndIf}

	#reset regView to standard 32bit
	SetRegView 32

	MessageBox MB_ICONINFORMATION|MB_OK "Microsoft Visual Studio, C++ compilers and a full Git setup (including SSH) need to be installed for a full OpenTwin Development environment"
    StrCpy $PortReturnChecker 0
	StrCpy $PublicIpSet 0
	StrCpy $PublicCertPageChecker 0
	StrCpy $GITHUB_DESKTOP_DEPLOYMENT_INSTALL_LOCATION "$PROGRAMFILES\GitHub Desktop Deployment"
		#standard and apparently unchangeable path of github desktop deployment application

	StrCpy "$ROOTDIR" "$WINDIR" 2
	

FunctionEnd



########################################################################

Function NetworkModePage
	StrCpy $PublicIpSet 0
	!insertmacro MUI_HEADER_TEXT "OpenTwin Server Settings" "Select your desired network configuration for OpenTwin. Select between a local runtime via your localhost_radio_btn adress or a remote server setup via a public IP adress"		
		nsDialogs::Create 1018
		Pop $Dialog

		${If} $Dialog == error
			Abort
		${EndIf}
		
		${NSD_CreateLabel} 0 0 100% 12u "Please select your desired network configuration"

		${NSD_CreateLabel} 25 30u 100% 10% "Local Setup for OpenTwin (via localhost)"
		${NSD_CreateRadioButton} 25 45u 25% 10% "localhost 127.0.0.1"
		Pop $localhost_radio_btn
		${NSD_Check} $localhost_radio_btn
		${NSD_OnClick} $localhost_radio_btn TogglePublicIPField

		${NSD_CreateLabel} 25 85u 100% 10% "Network Setup for OpenTwin (via Public IP)"
		${NSD_CreateRadioButton} 25 100u 25% 10% "Public IP"
		Pop $RadioButtonPublic
		
		;NSD_CreateIPAddress creates a font bug in the UI
		${NSD_CreateIPAddress} 150 100u 50% 10% ""
		Pop $public_ip_field
			;fix font bug
			CreateFont $R0 "$(^Font)" "$(^FontSize)"
			SendMessage $public_ip_field ${WM_SETFONT} $R0 1 
			;fix end
		#${NSD_CreateText} 150 120u 50% 10% "Public IP"
		#Pop $public_ip_field
		EnableWindow $public_ip_field 0
		${NSD_OnClick} $RadioButtonPublic TogglePublicIPField

    #restore state of the page when returning to it
    ${If} $localhost_radio_btn_state == ${BST_CHECKED}
        ${NSD_Check} $localhost_radio_btn
		StrCpy $PublicIpSet 0
        ${NSD_Uncheck} $RadioButtonPublic
    ${ElseIf} $RadioButtonPublic_state == ${BST_CHECKED}
        ${NSD_Check} $RadioButtonPublic
        ${NSD_Uncheck} $localhost_radio_btn
        EnableWindow $public_ip_field 1
    ${EndIf}

    ${NSD_SetText} $public_ip_field $public_ip_field_content

	nsDialogs::Show
FunctionEnd

	Function TogglePublicIPField
		${NSD_GetState} $RadioButtonPublic $0
		${If} $0 == ${BST_CHECKED}
			EnableWindow $public_ip_field 1
		${Else}
			EnableWindow $public_ip_field 0
		${EndIf}
	FunctionEnd

	Function OnNetworkLeave
		Pop $0
		#save states for returning to the page
		${NSD_GetChecked} $localhost_radio_btn $localhost_radio_btn_state
		${NSD_GetChecked} $RadioButtonPublic $RadioButtonPublic_state
		${NSD_GetText} $public_ip_field $public_ip_field_content


		${NSD_GetChecked} $localhost_radio_btn $0
		${NSD_GetChecked} $RadioButtonPublic $1
		${If} $0 <> ${BST_UNCHECKED}
			StrCpy $NetworkModeSelection "127.0.0.1"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_SERVICES_ADDRESS" "$NetworkModeSelection"

			StrCpy $STANDARD_CERT_PATH "$INSTDIR\Certificates"
			StrCpy $OPEN_TWIN_SERVICES_ADDRESS $NetworkModeSelection
			
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_CERTS_PATH" "$STANDARD_CERT_PATH"

			SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000

		${ElseIf} $1 <> ${BST_UNCHECKED}
			${NSD_GetText} $public_ip_field $0
			${If} $0 == "0.0.0.0"	#check if ip field is empty or 0.0.0.0
				MessageBox MB_ICONSTOP|MB_OK "Please enter a valid IP address before proceeding."
				Abort
			${Else}
				StrCpy $NetworkModeSelection $0
				StrCpy $PublicIPSet 1 #let the installer know, that a public IP has been set

				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_SERVICES_ADDRESS" "$NetworkModeSelection"
				SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000
				StrCpy $OPEN_TWIN_SERVICES_ADDRESS $NetworkModeSelection
			${EndIf}
		${Else}
			Abort
		${EndIf}

	FunctionEnd


Function PublicIPCertificate
	${If} $PublicIpSet <> 1
		Goto SkipPublicIPCertPage
	${EndIf}

	!insertmacro MUI_HEADER_TEXT "Public IP Certificate" "Server certificates need to be created for the public server IP address. In order for the script to be stored correctly, it needs a path"		
		nsDialogs::Create 1018
		Pop $Dialog

		${If} $Dialog == error
			Abort
		${EndIf}

	${NSD_CreateLabel} 0 0 100% 24u "Please specify the location of where you want to store your server certificate file"

	${NSD_CreateLabel} 0 50 100% 10u "Directory to store your server's certificate file:"
	${NSD_CreateText} 0 70 75% 12u "$ROOTDIR\OpenTwin\Certificates"
		Pop $DirHandleCert
		${NSD_GetText} $DirHandleCert $PUBLIC_CERT_PATH
		#SendMessage $DirHandleDB ${EM_SETREADONLY} 1 0 #set DirHandleDB to read only
	${NSD_CreateBrowseButton} 350 68 65u 15u "Browse..."
		Pop $BrowseButton
	${NSD_OnClick} $BrowseButton SelectCertDirectory

	nsDialogs::Show
	SkipPublicIPCertPage:
FunctionEnd

	Function SelectCertDirectory
		nsDialogs::SelectFolderDialog "Select a Directory" ""
		Pop $0 	#Get selected directory
		${If} $0 != error
			SendMessage $DirHandleCert ${WM_SETTEXT} 0 "STR:$0" #assign selected directory to DirHandle
			StrCpy $PUBLIC_CERT_PATH $0
		${EndIf}		
	FunctionEnd


	Function OnPublicCertficateLeave

		StrCpy $PublicCertPageChecker 1 

		WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_CERTS_PATH" "$PUBLIC_CERT_PATH"
		
		SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000

	FunctionEnd

/* NETWORK INFO PAGE (DISABLED)
	Function NetworkInfoPage
		nsDialogs::Create 1018
		Pop $Dialog

		${If} $Dialog == error
			Abort
		${EndIf}
		
		${NSD_CreateLabel} 0 0 100% 12u "You selected the following IP:"
		${NSD_CreateLabel} 0 15u 100% 12u $MONGODB_CUSTOM_PORT
		
		nsDialogs::Show
	FunctionEnd
*/

Function PortPage
	!insertmacro MUI_HEADER_TEXT "OpenTwin Port Settings" "Please review and choose your port setup"
	nsDialogs::Create 1018
	Pop $Dialog

	${If} $Dialog == error
		Abort
	${EndIf}
	
	${NSD_CreateLabel} 0 0 100% 15u "Please select and specify your MongoDB Network ports to be modified for OpenTwin. If not changed, the services will be assigned automatic ports"
	
	${NSD_CreateRadioButton} 25 20u 50% 10% "Apply standard values"
	Pop $standardPortValuesRadioBtn
	${NSD_Check} $standardPortValuesRadioBtn
	${NSD_OnClick} $standardPortValuesRadioBtn TogglePortFields
	
	${NSD_CreateRadioButton} 25 35u 25% 10% "Customize values"
	Pop $customPortValuesRadioBtn
	${NSD_OnClick} $customPortValuesRadioBtn TogglePortFields
	
	${NSD_CreateLabel} 30 50u 100u 10% "Authentication Service Port"
		${NSD_CreateNumber} 220 50u 10% 8% "8092"
		Pop $auth_port
		EnableWindow $auth_port 0
			SendMessage $auth_port ${EM_SETLIMITTEXT} 5 0 ;limit input to just 5 chars
			
	${NSD_CreateLabel} 30 65u 100u 10% "Global Session Service Port"
		${NSD_CreateNumber} 220 65u 10% 8% "8091"
		Pop $gss_port
		EnableWindow $gss_port 0
			SendMessage $gss_port ${EM_SETLIMITTEXT} 5 0
		
	${NSD_CreateLabel} 30 80u 100u 10% "Local Session Service Port"
		${NSD_CreateNumber} 220 80u 10% 8% "8093"
		Pop $lss_port
		EnableWindow $lss_port 0
			SendMessage $lss_port ${EM_SETLIMITTEXT} 5 0
		
	${NSD_CreateLabel} 30 95u 100u 10% "Global Directory Service Port"
		${NSD_CreateNumber} 220 95u 10% 8% "9094"
		Pop $gds_port
		EnableWindow $gds_port 0
			SendMessage $gds_port ${EM_SETLIMITTEXT} 5 0
		
	${NSD_CreateLabel} 30 110u 100u 10% "Local Directory Service Port"
		${NSD_CreateNumber} 220 110u 10% 8% "9095"
		Pop $lds_port
		EnableWindow $lds_port 0		
			SendMessage $lds_port ${EM_SETLIMITTEXT} 5 0

	${NSD_CreateLabel} 30 125u 100u 10% "MongoDB Port"
		${NSD_CreateNumber} 220 125u 10% 8% "27017"
		Pop $MongoDBCustomPortField
		EnableWindow $MongoDBCustomPortField 0		
			SendMessage $MongoDBCustomPortField ${EM_SETLIMITTEXT} 5 0


	${If} $PortReturnChecker <> 0
		${If} $standardPortValuesRadioBtnState == ${BST_CHECKED}
			${NSD_Check} $standardPortValuesRadioBtn
			${NSD_Uncheck} $customPortValuesRadioBtn
		${ElseIf} $customPortValuesRadioBtnState == ${BST_CHECKED}
			${NSD_Check} $customPortValuesRadioBtn
			${NSD_Uncheck} $standardPortValuesRadioBtn
			EnableWindow $auth_port 1
			EnableWindow $gss_port 1
			EnableWindow $lss_port 1
			EnableWindow $gds_port 1
			EnableWindow $lds_port 1
			EnableWindow $MongoDBCustomPortField 1
		${EndIf}

		${NSD_SetText} $auth_port $auth_port_content
		${NSD_SetText} $gss_port $gss_port_content
		${NSD_SetText} $lss_port $lss_port_content
		${NSD_SetText} $gds_port $gds_port_content
		${NSD_SetText} $lds_port $lds_port_content
		${NSD_SetText} $MongoDBCustomPortField $MongoDBCustomPortField_content

	${EndIf}
	nsDialogs::Show
FunctionEnd

	Function TogglePortFields
		${NSD_GetState} $customPortValuesRadioBtn $0
		${If} $0 == ${BST_CHECKED}
			EnableWindow $auth_port 1
			EnableWindow $gss_port 1
			EnableWindow $lss_port 1
			EnableWindow $gds_port 1
			EnableWindow $lds_port 1
			EnableWindow $MongoDBCustomPortField 1
		${Else}
			EnableWindow $auth_port 0
			EnableWindow $gss_port 0
			EnableWindow $lss_port 0
			EnableWindow $gds_port 0
			EnableWindow $lds_port 0
			EnableWindow $MongoDBCustomPortField 0
		${EndIf}
	FunctionEnd

	Function OnPortLeave
		StrCpy $PortReturnChecker 1

		${NSD_GetChecked} $standardPortValuesRadioBtn $standardPortValuesRadioBtnState
		${NSD_GetChecked} $customPortValuesRadioBtn $customPortValuesRadioBtnState

		${NSD_GetText} $auth_port $auth_port_content
		${NSD_GetText} $gss_port $gss_port_content
		${NSD_GetText} $lss_port $lss_port_content
		${NSD_GetText} $gds_port $gds_port_content
		${NSD_GetText} $lds_port $lds_port_content	
		${NSD_GetText} $MongoDBCustomPortField $MongoDBCustomPortField_content

		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_AUTH_PORT"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GSS_PORT"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LSS_PORT"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GDS_PORT"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LDS_PORT"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_MONGODB_ADDRESS"

		${NSD_GetState} $standardPortValuesRadioBtn $0
		StrCmp $0 1 standardPortRadioBtnChecked		;assign standard values if standard values is ticked
			
		${NSD_GetState} $customPortValuesRadioBtn $0
		StrCmp $0 1 customPortRadioBtnChecked		;else If get the input from the text fields
		Goto EndCheck
		
		standardPortRadioBtnChecked:
			StrCpy $auth_entry "8092" 
			StrCpy $gss_entry "8091"
			StrCpy $lss_entry "8093"
			StrCpy $gds_entry "9094"
			StrCpy $lds_entry "9095"
			StrCpy $MONGODB_CUSTOM_PORT "27017"

			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_AUTH_PORT" "$auth_entry"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GSS_PORT" "$gss_entry"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LSS_PORT" "$lss_entry"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GDS_PORT" "$gds_entry"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LDS_PORT" "$lds_entry"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_MONGODB_ADDRESS" "tls@$NetworkModeSelection:$MONGODB_CUSTOM_PORT"
		Goto EndCheck
				
		customPortRadioBtnChecked:
			${NSD_GetText} $auth_port $0
			${NSD_GetText} $gss_port $1
			${NSD_GetText} $lss_port $2
			${NSD_GetText} $gds_port $3
			${NSD_GetText} $lds_port $4
			${NSD_GetText} $MongoDBCustomPortField $5

			#check for empty fields
			${If} $0 == "" 
			${OrIf} $1 == "" 
			${OrIf} $2 == "" 
			${OrIf} $3 == "" 
			${OrIf} $4 == ""
			${OrIf} $5 == ""
				MessageBox MB_ICONSTOP|MB_OK "Please fill all port fields before proceeding."
				Abort
			${EndIf}

			#check for valid range of inputs (1-65535)
			${If} $0 > 65535 
				${OrIf} $0 < 1
			${OrIf} $1 > 65535
				${OrIf} $1 < 1
			${OrIf} $2 > 65535
				${OrIf} $2 < 1
			${OrIf} $3 > 65535
				${OrIf} $3 < 1
			${OrIf} $4 > 65535
				${OrIf} $4 < 1
			${OrIf} $5 > 65535
				${OrIf} $5 < 1
				MessageBox MB_ICONSTOP|MB_OK "One or more port numbers exceed the valid range of 1 - 65535."
				Abort
			${EndIf}	

			#check for redundant entries
			${If} $0 == $1 
			${OrIf} $0 == $2 
			${OrIf} $0 == $3 
			${OrIf} $0 == $4 
			${OrIf} $0 == $5

			${OrIf} $1 == $2 
			${OrIf} $1 == $3 
			${OrIf} $1 == $4
			${OrIf} $1 == $5 

			${OrIf} $2 == $3 
			${OrIf} $2 == $4 
			${OrIf} $2 == $5

			${OrIf} $3 == $4
			${OrIf} $3 == $5

			${OrIf} $4 == $5
				MessageBox MB_ICONSTOP|MB_OK "Port values must be unique. Please ensure no two ports are the same"
				Abort
			${EndIf}
				${NSD_GetText} $auth_port $auth_entry
				${NSD_GetText} $gss_port $gss_entry
				${NSD_GetText} $lss_port $lss_entry
				${NSD_GetText} $gds_port $gds_entry
				${NSD_GetText} $lds_port $lds_entry
				${NSD_GetText} $MongoDBCustomPortField $MONGODB_CUSTOM_PORT

				#set new custom env variable values and notify the system every time
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_AUTH_PORT" "$auth_entry"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GSS_PORT" "$gss_entry"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LSS_PORT" "$lss_entry"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GDS_PORT" "$gds_entry"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LDS_PORT" "$lds_entry"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_MONGODB_ADDRESS" "tls@$NetworkModeSelection:$MONGODB_CUSTOM_PORT"
		EndCheck:	
		
		SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000		

	FunctionEnd


/*# PORT SELECTION INFO PAGE (DISABLED)
	Function PortInfoPage
		nsDialogs::Create 1018
		Pop $Dialog

		${If} $Dialog == error
			Abort
		${EndIf}
		
		${NSD_CreateLabel} 0 0 100% 12u "You selected the following Ports:"
			${NSD_CreateLabel} 0 15u 100% 12u $auth_entry
				${NSD_CreateLabel} 55 15u 100% 12u "Auth Port"
			${NSD_CreateLabel} 0 30u 100% 12u $gss_entry
				${NSD_CreateLabel} 55 30u 100% 12u "GSS Port"
			${NSD_CreateLabel} 0 45u 100% 12u $lss_entry
				${NSD_CreateLabel} 55 45u 100% 12u "LSS Port"
			${NSD_CreateLabel} 0 60u 100% 12u $gds_entry
				${NSD_CreateLabel} 55 60u 100% 12u "GDS Port"
			${NSD_CreateLabel} 0 75u 100% 12u $lds_entry
				${NSD_CreateLabel} 55 75u 100% 12u "LDS Port"
			${NSD_CreateLabel} 0 90u 100% 12u $MONGODB_CUSTOM_PORT
				${NSD_CreateLabel} 55 90u 100% 12u "Mongo Custom"
		
		nsDialogs::Show
	FunctionEnd
*/

# DATABASE DIRECTORY OPERATIONS (ACTIVE)
	Function DatabaseEntry
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
			${NSD_GetText} $DirHandleMainInstall $MONGODB_INSTALL_PATH
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
				StrCpy $MONGODB_INSTALL_PATH $0
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


/* DATABASE INFO PAGE (DISABLED)
	Function DatabaseInfo
		nsDialogs::Create 1018
		Pop $Dialog

		${If} $Dialog == error
			Abort
		${EndIf}

		${NSD_CreateLabel} 0 0 100% 10u "MONGODB_INSTALL_PATH:"
			${NSD_CreateLabel} 0 25 100% 30 $MONGODB_INSTALL_PATH

		${NSD_CreateLabel} 0 100 100% 10u "MONGODB_DB_PATH:"
			${NSD_CreateLabel} 0 125 100% 30 $MONGODB_DB_PATH

		${NSD_CreateLabel} 0 175 100% 10u "MONGODB_LOG_PATH:"
			${NSD_CreateLabel} 0 200 100% 30 $MONGODB_LOG_PATH
		nsDialogs::Show
	FunctionEnd
*/

Function BrowseDevRoot
	!insertmacro MUI_HEADER_TEXT "OpenTwin Dev Root" "Provide a developer root directory for the OpenTwin repository."
    nsDialogs::Create 1018
    Pop $Dialog

    ${If} $Dialog == error
        Abort
    ${EndIf}

	${NSD_CreateLabel} 0 0 100% 30u "Please specify a directory for storing the full OpenTwin Github repository. Please note, that the repository cloning process will at least require 100GB of space. The same path appended with '\OpenTwin\Deployment' will be used to extract all OpenTwin Deployment files to."
	${NSD_CreateGroupBox} 5% 60u 90% 34u "OpenTwin Dev Root Path"
    Pop $0

        ${NSD_CreateDirRequest} 15% 74u 49% 12u ${DEV_ROOT_DIR}
        Pop $DevRootDirRequest

        ${NSD_CreateBrowseButton} 65% 73u 20% 14u "Browse..."
        Pop $0
        ${NSD_OnClick} $0 OnDevRootDirBrowse

    nsDialogs::Show

FunctionEnd

	Function OnDevRootDirBrowse
		${NSD_GetText} $DevRootDirRequest $0
		nsDialogs::SelectFolderDialog "Select Dev Root Directory" "$0" 
		Pop $0
		${If} $0 != error
			${NSD_SetText} $DevRootDirRequest "$0"
		${EndIf}
	FunctionEnd

	Function OnBrowseDevRootLeave
		${NSD_GetText} $DevRootDirRequest $DEV_ROOT
		IfFileExists "$DEV_ROOT"  skip_dev_root_init DEV_ROOT_init
		DEV_ROOT_init:
			CreateDirectory "$DEV_ROOT"
		skip_dev_root_init:
			StrCpy $OPENTWIN_DEV_ROOT "$DEV_ROOT\OpenTwin"
			StrCpy $OPENTWIN_THIRDPARTY_ROOT "$DEV_ROOT\ThirdParty"
				#assign variable paths but don't create them yet!
				#git wouldn't clone if a path already exists!
			StrCpy $INSTDIR "$OPENTWIN_DEV_ROOT\Deployment"
			StrCpy "$TempToolChain" ${TEMP_TOOLCHAIN_DIR}
	FunctionEnd

Function WaitForShortcut
	Pop $2 #application name string
	Pop $0 #path to shortcut
    
    #initialize counter
    StrCpy $1 0

    loop_init:
        IfFileExists $0 done loop
	loop:
        IntOp $1 $1 + 1
        #check for timeout
        ${If} $1 > 60
			Goto timeout
		${EndIf} 

        Sleep 999
        Goto loop_init
    
    done:
        #file exists, return to section
        Return

    timeout:
        #60 sec timeout
        MessageBox MB_OK|MB_ICONEXCLAMATION "$2 Installation timed out. Proceeding with installation now but skipping this step."

FunctionEnd

; MUI Settings
	!define MUI_ABORTWARNING
	!define MUI_ICON ${MUI_ICON_PATH}

	!define MUI_UNICON ${MUI_UNICON_PATH}

	; Welcome page
	!insertmacro MUI_PAGE_WELCOME
	; License page
	!define MUI_LICENSEPAGE_CHECKBOX
	!insertmacro MUI_PAGE_LICENSE ${LICENSE_FILE_PATH}

	; Components page
	!define MUI_COMPONENTSPAGE_SMALLDESC
	!insertmacro MUI_PAGE_COMPONENTS

	; Page calls - order is relevant!
	#page custom DatabaseInfo #MongoDB paths debug page
	Page custom BrowseDevRoot OnBrowseDevRootLeave
	Page custom DatabaseEntry
	Page custom NetworkModePage OnNetworkLeave
	Page custom PublicIPCertificate OnPublicCertficateLeave
	#Page custom NetworkInfoPage #Network debug page
	Page custom PortPage OnPortLeave
	#Page custom PortInfoPage #port debug page

	; Start menu page
	var ICONS_GROUP
	!define MUI_STARTMENUPAGE_NODISABLE
	!define MUI_STARTMENUPAGE_DEFAULTFOLDER "Launch OpenTwin"
	!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
	!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
	!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
	!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
	; Instfiles page
	!insertmacro MUI_PAGE_INSTFILES
	; Finish page
	#!define MUI_FINISHPAGE_RUN "$INSTDIR\OpenTwin_local.bat"

	!define MUI_TEXT_FINISH_INFO_TEXT "Installation complete. All programs and dependencies have been setup and installed successfully. Click on Finish to close the installer."

	!insertmacro MUI_PAGE_FINISH

	; Uninstaller pages
	!insertmacro MUI_UNPAGE_INSTFILES

	; Language files
	!insertmacro MUI_LANGUAGE "English"
; MUI end ------

Name "${PRODUCT_NAME}"
OutFile "Install_OpenTwin_dev.exe"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails hide
ShowUnInstDetails hide

Section "-Extract Installer Tools (Required)" SEC01
	SectionIn RO ;read only section
	SetOutPath ${TEMP_TOOLCHAIN_DIR}

	#File command can only use relative to script location paths
	#variables won't be recoginzed unfortunately
	#    ".\" 	 = the script location itself
	#    "..\" 	 = one directory up
	#    "..\..\"  = two directories up etc.

	DetailPrint "Extracting toolchain..."
	File /r "..\..\..\..\ThirdParty\Installer_Tools\ThirdParty\dev\*.*"
	File /r "..\..\..\..\ThirdParty\Installer_Tools\ThirdParty\shared\*.*"
	File /r "..\python\dist\*.*"
	File /r "..\helper\*.*"
	File /r "..\javascript\*.*"

SectionEnd


SectionGroup /e "OpenTwin"
	Section "!Clone OpenTwin Repository" SEC02
		MessageBox MB_ICONINFORMATION|MB_OK "This step of the installation will clone the OpenTwin repository on GitHub. Press OK to continue."

		SetRegView 64
		ReadRegStr $0 HKLM "SOFTWARE\GitForWindows" "InstallPath"
		StrCpy $GitInstallPath $0
		#MessageBox MB_ICONINFORMATION|MB_OK "GitHub Path: $GitInstallPath" #debug
		SetRegView 32

		IfFileExists "$DEV_ROOT" DEV_ROOT_Exists DEV_ROOT_NotExists

		DEV_ROOT_Exists:
			DetailPrint "Cloning repositories..."
			ExpandEnvStrings $0 %COMSPEC%
				ExecWait '"$0" /k "cd "$DEV_ROOT" && git clone ${OPENTWIN_REPO_GITADDRESS} && git clone ${THIRDPARTY_REPO_GITADDRESS} && exit"'
				#ExecWait '"$0" /k "cd "$DEV_ROOT" && git clone ${OPENTWIN_REPO_GITADDRESS} && exit"'
			Goto done_cloning
		
		DEV_ROOT_NotExists:
			DetailPrint "DEV_ROOT path doesn't exist yet! Creating path..." #if for some reason this ever happens
				CreateDirectory $DEV_ROOT
			Goto DEV_ROOT_Exists
		
		done_cloning:
	SectionEnd

	Section "OpenTwin Deployment Files (Required)" SEC03
		SectionIn RO ;read only section
		SetOutPath "$INSTDIR"
		SetOverwrite ifnewer

		DetailPrint "Extracting OpenTwin Deployment files..."

		File /r "..\..\..\Deployment\*.*"

		#try with refreshenv.cmd
		${If} $PublicIpSet <> 0 #public IP was set
			ExpandEnvStrings $0 %COMSPEC%
				ExecWait '"$0" /c "START /WAIT /MIN cmd.exe /c ""$TempToolChain\RefreshEnv.cmd" && cd "$INSTDIR\Certificates" && "$INSTDIR\Certificates\CreateServerCertificate.bat""" '
		${Else}
			Goto +2
		${EndIf}

		DetailPrint "Installing VC Redistributable..."
		# The 32bit version is required for Apache
		ExecWait '"$TempToolChain\VC_redist.x86.exe" /silent'
		ExecWait '"$TempToolChain\VC_redist.x64.exe" /silent'

		DetailPrint "Setting environment variables..."

		WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPENTWIN_DEV_ROOT" "$OPENTWIN_DEV_ROOT"
			SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000

		WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPENTWIN_THIRDPARTY_ROOT" "$OPENTWIN_THIRDPARTY_ROOT"
			SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000		

		; Shortcuts
		!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
		CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\OpenTwin.lnk" "$INSTDIR\OpenTwin_local.bat" "" ${OPENTWIN_APP_ICON}
		CreateShortCut "$DESKTOP\OpenTwin.lnk" "$INSTDIR\OpenTwin_local.bat" "" ${OPENTWIN_APP_ICON}
		!insertmacro MUI_STARTMENU_WRITE_END
	SectionEnd


	Section "MongoDB Setup" SEC04
		AddSize 738000
		; /i = package to install
		; /qn = quiet install
		; INSTALLLOCATION = install directory
		
		#this execwait would have to be enabled again if the custom install location can be used again
		#ExecWait 'msiexec /l*v mdbinstall.log  /qb /i "$INSTDIR\Tools\mongodb-windows-x86_64-4.4.28-signed.msi" INSTALLLOCATION="$MONGODB_INSTALL_PATH" SHOULD_INSTALL_COMPASS="0" ADDLOCAL="ServerService,Client"'
		
		DetailPrint "Running MongoDB installation scripts..."

		ExecWait 'msiexec /l*v mdbinstall.log  /qb /i "$TempToolChain\mongodb-windows-x86_64-4.4.28-signed.msi" INSTALLLOCATION="$MONGODB_INSTALL_PATH" SHOULD_INSTALL_COMPASS="0" ADDLOCAL="ServerService,Client"'		
		ExecWait 'msiexec /qb /i "$TempToolChain\mongodb-compass-1.42.3-win32-x64.msi"'
		DetailPrint "Waiting for MongoDB Compass Application..."
			Push ${DEFAULT_MONGODB_COMPASS_PATH}
			Push "MongoDB Compass"
			Call WaitForShortcut
		DetailPrint "Done."

		nsExec::ExecToLog 'net stop "MongoDB"'	
		
		##########################################
		# call for python scripts via $INSTDIR
		##########################################
		DetailPrint "Running scripts..."
		# update the mongodB config file without authentication
		ExecWait '"$TempToolChain\ConfigMongoDBNoAuth.exe" "$MONGODB_INSTALL_PATH\bin\mongod.cfg" "$MONGODB_DB_PATH" "$MONGODB_LOG_PATH" $NetworkModeSelection'

		#set directory permissions for the mongoDB service
		ExecWait '"$TempToolChain\SetPermissions.exe" "$MONGODB_DB_PATH" "$MONGODB_LOG_PATH"'

		# restarting mongoDB service
		nsExec::ExecToLog 'net start "MongoDB"'
		# 'net' command waits for the service to be stopped/started automatically
		# no additional checks needed

		# call for js script to paste admin user creation
		ExpandEnvStrings $0 %COMSPEC%
			ExecWait '"$0" /c "START /WAIT /MIN cmd.exe /c " "$MONGODB_INSTALL_PATH\bin\mongo.exe" < "$TempToolChain\db_admin.js" " "'

		nsExec::ExecToLog 'net stop "MongoDB"'	

		# mongoDB_storage_script_wauth.py
		${If} $PublicIpSet <> 0
			ExecWait '"$TempToolChain\ConfigMongoDBWithAuth.exe" "$MONGODB_INSTALL_PATH\bin\mongod.cfg" "$PUBLIC_CERT_PATH\certificateKeyFile.pem" "$MONGODB_CUSTOM_PORT"'
			ExpandEnvStrings $0 %COMSPEC%
				ExecWait '"$0" /c "START /WAIT /MIN cmd.exe /c "certutil -addstore root "$PUBLIC_CERT_PATH\ca.pem"""" '	
		${Else}
			ExecWait '"$TempToolChain\ConfigMongoDBWithAuth.exe" "$MONGODB_INSTALL_PATH\bin\mongod.cfg" "$STANDARD_CERT_PATH\certificateKeyFile.pem" "$MONGODB_CUSTOM_PORT"'
			ExpandEnvStrings $0 %COMSPEC%
				ExecWait '"$0" /c "START /WAIT /MIN cmd.exe /c "certutil -addstore root "$STANDARD_CERT_PATH\ca.pem"""" '
		${EndIf}
		
		DetailPrint "Restart services..."
		nsExec::ExecToLog 'net start "MongoDB"'	
		
	SectionEnd
SectionGroupEnd

SectionGroup /e "Git" 	
	Section "Git GUI Frontend" SEC05
		AddSize 613000
		DetailPrint "Installing Github Desktop Environment"
		ExpandEnvStrings $0 %COMSPEC%
			ExecWait 'msiexec.exe /i "$TempToolChain\GitHubDesktopSetup-x64.msi" /quiet'
		ExecWait '"$GITHUB_DESKTOP_DEPLOYMENT_INSTALL_LOCATION\GitHubDesktopDeploymentTool.exe" /silent'

		DetailPrint "Waiting for GitHub Desktop..."
			Push "$DESKTOP\GitHub Desktop.lnk"
			Push "GitHub Desktop"
			Call WaitForShortcut
		DetailPrint "Done."
	SectionEnd
SectionGroupEnd

#Visual studio is a lenghty online installer that runs its own set of operations - might be problematic during installer runtime
#requires internet

Section "Install Rust" SEC06_1
	AddSize 114000
	DetailPrint "Installing Rust..."
	ExecWait '"$TempToolChain\rustup-init.exe" -y'	#run rustup setup without confirmation prompt (-y)

	DetailPrint "Running Rust commands..."
	ExpandEnvStrings $0 %COMSPEC%
			ExecWait '"$0" /c "START /WAIT /MIN cmd.exe /c "rustup install stable latest" && "rustup default stable latest" && "rustup update" " '
SectionEnd


Section "Install PostMan 64-bit" SEC07
	AddSize 535000
	DetailPrint "Installing PostMan..."
		ExecWait '"$TempToolChain\Postman-win64-Setup.exe" /silent'	#silent installs postman - postman installation is mostly silent already
																	#the '/silent' switch surpresses the automatic app launch after installation
		DetailPrint "Waiting for Postman..."
			Push "$DESKTOP\Postman.lnk"
			Push "Postman"
			Call WaitForShortcut
		DetailPrint "Done."
SectionEnd

Section "Install Node.js and yarn" SEC08
	AddSize 83100
	DetailPrint "Installing Node.js"
		ExecWait 'msiexec.exe /i "$TempToolChain\node-v20.11.1-x64.msi" /quiet'
		#ExecWait 'msiexec.exe /i "$TempToolChain\node-v20.11.1-x64.msi" /passive' 	# '/passive' includes a progress bar but doesn't need any input 
																					# use "INSTALLDIR="path\to\NodeJS" to install to a specific location 
	DetailPrint "Installing yarn..."
		ExpandEnvStrings $0 %COMSPEC%
				ExecWait '"$0" /c "START /WAIT /MIN cmd.exe /c " "$TempToolChain\RefreshEnv.cmd" && npm install --global yarn" " '
SectionEnd

SectionGroup /e "Python & Sphinx"
	Section "Install Python 3.9" SEC09
		AddSize 151000
		DetailPrint "Installing Python 3.9..."
			ExecWait '"$TempToolChain\python-3.9.0-amd64.exe" /quiet InstallAllUsers=0 PrependPath=1 Include_test=0'
			# /quiet 			- installs python silently
			# /passive			- install with only a progress bar
			# InstallAllUsers	- installs python for all users
			# PrependPath 		- Prepend install and scripts directories to PATH and add .PY to PATHEXT
			# Include_test 		- Install standard library test suite
			# these settings replicate a standard python 3 installation
			# (via https://docs.python.org/3/using/windows.html)
	SectionEnd

	Section "Install Sphinx" SEC09_1
		AddSize 22000
		DetailPrint "Installing Sphinx..."
			ExpandEnvStrings $0 %COMSPEC%
			#ExecWait '"$0" /k "START /WAIT /MIN cmd.exe /k " "$TempToolChain\RefreshEnv.cmd" && pip install sphinx && pip install sphinx_rtd_theme" " '
			ExecWait 'cmd.exe /c "title Installing Sphinx and ReadTheDocs Theme && "$TempToolChain\RefreshEnv.cmd" && pip install -U sphinx && pip install -U sphinx_rtd_theme " '
	SectionEnd

	Section "Install Pyinstaller" SEC09_2
		AddSize 106
		DetailPrint "Installing Pyinstaller..."
			ExpandEnvStrings $0 %COMSPEC%
			#ExecWait '"$0" /k "START /WAIT /MIN cmd.exe /k " "$TempToolChain\RefreshEnv.cmd" && pip install pyinstaller" " '
			ExecWait 'cmd.exe /c "title Installing pyinstaller && "$TempToolChain\RefreshEnv.cmd" && pip install -U pyinstaller " '
	SectionEnd
SectionGroupEnd

Section "-CleanupTasks"
	SectionIn RO
	DetailPrint "Refreshing Environment..."
	ExpandEnvStrings $0 %COMSPEC%
		ExecWait '"$0" /c "START /WAIT /MIN cmd.exe /c " "$TempToolChain\RefreshEnv.cmd"'
	DetailPrint "Cleaning up..."
	RMDir /r ${TEMP_TOOLCHAIN_DIR}
	RMDir /r $GITHUB_DESKTOP_DEPLOYMENT_INSTALL_LOCATION
	
SectionEnd

Section -AdditionalIcons
  	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  		CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\Uninstall_OpenTwin.exe" "" ${OPENTWIN_UNAPP_ICON}
 	!insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
	WriteUninstaller "$INSTDIR\Uninstall_OpenTwin.exe"
	WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\OpenTwin.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayName" "${PRODUCT_NAME}"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayIcon" "$INSTDIR\OpenTwin.exe,0"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "UninstallString" '"$INSTDIR\Uninstall_OpenTwin.exe"'
	WriteRegDWORD HKLM "${REGPATH_UNINSTSUBKEY}" "NoModify" 1
	WriteRegDWORD HKLM "${REGPATH_UNINSTSUBKEY}" "NoRepair" 1

SectionEnd

#Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "Clones both required GitHub repositories under https://github.com/OT-OpenTwin into the OpenTwin Developer directory"

	!insertmacro MUI_DESCRIPTION_TEXT ${SEC03} "OpenTwin Deployment Files - Install all required OpenTwin files"
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC04} "MongoDB Setup - Install MongoDB and MongoDB Compass. Set up all configurations for OpenTwin"

	!insertmacro MUI_DESCRIPTION_TEXT ${SEC05} "Install Git's graphical UI"

	!insertmacro MUI_DESCRIPTION_TEXT ${SEC06_1} "Install Rust (only in combination with Visual Studio 2022!)"

	!insertmacro MUI_DESCRIPTION_TEXT ${SEC07} "Install the 64-bit version of PostMan"
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC08} "Install Node.js and yarn"

	!insertmacro MUI_DESCRIPTION_TEXT ${SEC09} "Install a standard installation of Python 3.9"
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC09_1} "Install Sphinx via pip and the 'Read the docs' theme required for building the documentation"
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC09_2} "Install Pyinstaller via pip required for building OpenTwin Installer scripts"
!insertmacro MUI_FUNCTION_DESCRIPTION_END



#UNINSTALLER
Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +1 IDNO AbortUninstall
  	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Do you also want to uninstall MongoDB and all of its components?" IDYES +1 IDNO +3
	StrCpy $UNINSTALL_MONGODB_FLAG 1
		Goto +5
	StrCpy $UNINSTALL_MONGODB_FLAG 0
		Goto +3
AbortUninstall:
  Abort
FunctionEnd

Section Uninstall
#Delete ALL env variables set by the installer
	DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_AUTH_PORT"
	DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GSS_PORT"
	DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LSS_PORT"
	DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GDS_PORT"
	DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LDS_PORT"
	DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_MONGODB_ADDRESS"
	DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_CERTS_PATH"
	DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_SERVICES_ADDRESS"
	DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPENTWIN_DEV_ROOT"
	DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPENTWIN_THIRDPARTY_ROOT"
		
	SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000	

#===============================================================================================================================	
	
${If} $UNINSTALL_MONGODB_FLAG != 0
	nsExec::ExecToLog 'net stop "MongoDB"'
	ExecWait 'sc delete MongoDB'

	Delete "$MONGODB_INSTALL_PATH\*.*"
	RMDir /r "$MONGODB_INSTALL_PATH"
	RMDir /r ${MONGODB_DELETION_PATH}
${EndIf}


  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  Delete "$INSTDIR\*.*"
  RMDir /r "$INSTDIR"

  Delete "$PUBLIC_CERT_PATH\*.*"
  RMDir /r "$PUBLIC_CERT_PATH"

  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$DESKTOP\OpenTwin.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\OpenTwin.lnk"

  RMDir "$SMPROGRAMS\$ICONS_GROUP"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd