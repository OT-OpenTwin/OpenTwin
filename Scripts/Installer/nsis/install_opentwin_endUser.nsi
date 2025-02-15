/*

	OpenTwin End-User Installation Script
	This script installs OpenTwin and all its dependencies and runs external 
	python scripts to set those up

*/

!include "MUI.nsh"
!include LogicLib.nsh
!include nsDialogs.nsh
!include FileFunc.nsh

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
	Var DEFAULT_CERT_PATH

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
	
	Var admin_port
	Var admin_port_content
	
	Var download_port
	Var download_port_content

	Var MongoDBCustomPortField
	Var MongoDBCustomPortField_content
	Var MONGODB_CUSTOM_PORT
	Var mongodb_admin_pwd1
	Var mongodb_admin_pwd2

	Var PortReturnChecker

	Var auth_entry
	Var gss_entry
	Var lss_entry
	Var gds_entry
	Var lds_entry
	Var admin_entry
	Var download_entry
	
	# Certificate VARIABLES
	Var certs_path_content

	#Database (MongoDB) variables
	Var DirHandleMainInstall
	Var DirHandleDB
	Var DirHandleLog
	Var BrowseButton
	Var MONGODB_DB_PATH
	Var MONGODB_LOG_PATH
	#Var UNINSTALL_MONGODB_FLAG
	
	Var OPEN_TWIN_SERVICES_ADDRESS
	
	# For MongoDB Upgrade
	Var First_MONGODB_Install_FLAG
	Var UPGRADER_MONGODB_SERVICE_NAME
	Var UPGRADER_MONGODB_ADMIN_PASSWORD
	Var MONGODB_SETUP_MSG
	Var UPGRADER_MONGODB_INSTALL_PATH
	Var AdminPswHandle	
#=================================================================
#						END OF VARIABLES
#=================================================================



#=================================================================
#						GLOBAL DEFINES
#=================================================================
	!define PRODUCT_NAME "OpenTwin"
	BrandingText "OpenTwin"
	!define REGPATH_UNINSTSUBKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

	!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\OpenTwin.exe"
	!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
	!define PRODUCT_UNINST_ROOT_KEY "HKLM"
	!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

	# at script compile icons/images
	!define MUI_WELCOMEFINISHPAGE_BITMAP "..\Graphics\Wizard\openTwin_headerImage_01.bmp"
	!define MUI_ICON_PATH '"..\Graphics\Icons\openTwin_icon_48x48.ico"'
	!define MUI_UNICON_PATH '"..\Graphics\Icons\opentwin_uninstall_icon_48x48.ico"'

	# post installation icons/images
	!define OPENTWIN_APP_ICON '"$INSTDIR\icons\Application\OpenTwin.ico"'
	!define OPENTWIN_UNAPP_ICON '"$INSTDIR\icons\Application\opentwin_uninstall_icon_48x48.ico"'

	!define CREATE_CERTIFICATE_BATCH '"$INSTDIR\Certificates\CreateServerCertificate_custom.cmd"'
	!define DEFAULT_MONGODB_STORAGE_PATH '"C:\OT-DataStorage\data"'
	!define DEFAULT_MONGODB_LOG_PATH '"C:\OT-DataStorage\log"'
	!define DEFAULT_MONGODB_INSTALL_PATH '"$PROGRAMFILES64\MongoDB\Server\7.0"'
	!define MONGODB_DELETION_PATH '"$PROGRAMFILES64\MongoDB"'
	!define LICENSE_FILE_PATH '"..\..\..\LICENSE.md"'
	!define DEFAULT_MONGODB_ADMIN_PASSWORD "admin"

	!define MONGODB_STANDARD_PORT "27017"
	
	!define UPGRADER_ROOT "..\..\..\Tools\MongoDBUpgrader"
	!define HELPER_FILES_PATH "..\helper"
	!define MONGODB_UPGRADER_INST_EXE_PATH "$INSTDIR\Upgrader\MongoDB_Executable"
	!define MONGODB_INST_MSI_PATH "$INSTDIR\Upgrader\MongoDB_Installer" ;The relative position is used by the Upgrader.exe in the MongoDB_Executable folder
	!define MONGODB_INST_SERVER_PATH "$INSTDIR\Upgrader\MongoDB_Server"
	!define LOG_PATH "$INSTDIR\BuildInformation"
	
#=================================================================
#						MACROS
#=================================================================
!define StrStr "!insertmacro StrStr"
 
!macro StrStr ResultVar String SubString
  Push `${String}`
  Push `${SubString}`
  Call StrStr
  Pop `${ResultVar}`
!macroend
 
Function StrStr
/*After this point:
  ------------------------------------------
  $R0 = SubString (input)
  $R1 = String (input)
  $R2 = SubStringLen (temp)
  $R3 = StrLen (temp)
  $R4 = StartCharPos (temp)
  $R5 = TempStr (temp)*/
 
  ;Get input from user
  Exch $R0
  Exch
  Exch $R1
  Push $R2
  Push $R3
  Push $R4
  Push $R5
 
  ;Get "String" and "SubString" length
  StrLen $R2 $R0
  StrLen $R3 $R1
  ;Start "StartCharPos" counter
  StrCpy $R4 0
 
  ;Loop until "SubString" is found or "String" reaches its end
  loop:
    ;Remove everything before and after the searched part ("TempStr")
    StrCpy $R5 $R1 $R2 $R4
 
    ;Compare "TempStr" with "SubString"
    StrCmp $R5 $R0 done
    ;If not "SubString", this could be "String"'s end
    IntCmp $R4 $R3 done 0 done
    ;If not, continue the loop
    IntOp $R4 $R4 + 1
    Goto loop
  done:
 
/*After this point:
  ------------------------------------------
  $R0 = ResultVar (output)*/
 
  ;Remove part before "SubString" on "String" (if there has one)
  StrCpy $R0 $R1 `` $R4
 
  ;Return output to user
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Exch $R0
FunctionEnd

#=================================================================
#						END OF DEFINES
#=================================================================

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

 ; GetParent
 ; input, top of stack  (e.g. C:\Program Files\Foo)
 ; output, top of stack (replaces, with e.g. C:\Program Files)
 ; modifies no other variables.
 ;
 ; Usage:
 ;   Push "C:\Program Files\Directory\Whatever"
 ;   Call GetParent
 ;   Pop $R0
 ;   ; at this point $R0 will equal "C:\Program Files\Directory"
 
Function GetParent
 
  Exch $R0
  Push $R1
  Push $R2
  Push $R3
 
  StrCpy $R1 0
  StrLen $R2 $R0
 
  loop:
    IntOp $R1 $R1 + 1
    IntCmp $R1 $R2 get 0 get
    StrCpy $R3 $R0 1 -$R1
    StrCmp $R3 "\" get
  Goto loop
 
  get:
    StrCpy $R0 $R0 -$R1
 
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0
 
FunctionEnd

Function un.GetParent
 
  Exch $R0
  Push $R1
  Push $R2
  Push $R3
 
  StrCpy $R1 0
  StrLen $R2 $R0
 
  loop:
    IntOp $R1 $R1 + 1
    IntCmp $R1 $R2 get 0 get
    StrCpy $R3 $R0 1 -$R1
    StrCmp $R3 "\" get
  Goto loop
 
  get:
    StrCpy $R0 $R0 -$R1
 
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0
 
FunctionEnd

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


########################################################################

Function NetworkModePage
	StrCpy $PublicIpSet 0
	!insertmacro MUI_HEADER_TEXT "OpenTwin Server Settings" "Select your desired network configuration for OpenTwin. Select between a local runtime via your localhost adress or a remote server setup via a public IP adress"		
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

			StrCpy $DEFAULT_CERT_PATH "$INSTDIR\Certificates"
			StrCpy $OPEN_TWIN_SERVICES_ADDRESS $NetworkModeSelection
			
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_CERTS_PATH" "$DEFAULT_CERT_PATH"
			
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
	
	${If} $PublicCertPageChecker <> 0
		${NSD_SetText} $DirHandleCert $certs_path_content
	${EndIf}

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
		
		${NSD_GetText} $DirHandleCert $certs_path_content

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
	
	${NSD_CreateLabel} 0 0 100% 15u "Please select and specify your network ports to be used for OpenTwin."
	
	${NSD_CreateRadioButton} 0 15u 70% 10% "Apply standard port values"
	Pop $standardPortValuesRadioBtn
	${NSD_Check} $standardPortValuesRadioBtn
	${NSD_OnClick} $standardPortValuesRadioBtn TogglePortFields
	
	${NSD_CreateRadioButton} 0 35u 70% 10% "Customize port values"
	Pop $customPortValuesRadioBtn
	${NSD_OnClick} $customPortValuesRadioBtn TogglePortFields
	
	${NSD_CreateLabel} 0 55u 90u 10% "Authentication Service"
		${NSD_CreateNumber} 150 55u 10% 8% "8092"
		Pop $auth_port
		EnableWindow $auth_port 0
			SendMessage $auth_port ${EM_SETLIMITTEXT} 5 0 ;limit input to just 5 chars
			
	${NSD_CreateLabel} 230 55u 90u 10% "Global Session Service"
		${NSD_CreateNumber} 380 55u 10% 8% "8091"
		Pop $gss_port
		EnableWindow $gss_port 0
			SendMessage $gss_port ${EM_SETLIMITTEXT} 5 0
		
	${NSD_CreateLabel} 0 70u 90u 10% "Local Session Service"
		${NSD_CreateNumber} 150 70u 10% 8% "8093"
		Pop $lss_port
		EnableWindow $lss_port 0
			SendMessage $lss_port ${EM_SETLIMITTEXT} 5 0
		
	${NSD_CreateLabel} 230 70u 90u 12% "Global Directory Service"
		${NSD_CreateNumber} 380 70u 10% 8% "9094"
		Pop $gds_port
		EnableWindow $gds_port 0
			SendMessage $gds_port ${EM_SETLIMITTEXT} 5 0
		
	${NSD_CreateLabel} 0 85u 90u 10% "Local Directory Service"
		${NSD_CreateNumber} 150 85u 10% 8% "9095"
		Pop $lds_port
		EnableWindow $lds_port 0		
			SendMessage $lds_port ${EM_SETLIMITTEXT} 5 0

	${NSD_CreateLabel} 230 85u 90u 10% "MongoDB"
		${NSD_CreateNumber} 380 85u 10% 8% "27017"
		Pop $MongoDBCustomPortField
		EnableWindow $MongoDBCustomPortField 0		
			SendMessage $MongoDBCustomPortField ${EM_SETLIMITTEXT} 5 0

	${NSD_CreateLabel} 0 100u 90u 10% "Admin Panel"
		${NSD_CreateNumber} 150 100u 10% 8% "8000"
		Pop $admin_port
		EnableWindow $admin_port 0		
			SendMessage $admin_port ${EM_SETLIMITTEXT} 5 0

	${NSD_CreateLabel} 230 100u 90u 10% "Frontend Download"
		${NSD_CreateNumber} 380 100u 10% 8% "80"
		Pop $download_port
		EnableWindow $download_port 0		
			SendMessage $download_port ${EM_SETLIMITTEXT} 5 0

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
			EnableWindow $admin_port 1
			EnableWindow $download_port 1
		${EndIf}

		${NSD_SetText} $auth_port $auth_port_content
		${NSD_SetText} $gss_port $gss_port_content
		${NSD_SetText} $lss_port $lss_port_content
		${NSD_SetText} $gds_port $gds_port_content
		${NSD_SetText} $lds_port $lds_port_content
		${NSD_SetText} $MongoDBCustomPortField $MongoDBCustomPortField_content
		${NSD_SetText} $admin_port $admin_port_content
		${NSD_SetText} $download_port $download_port_content

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
			EnableWindow $admin_port 1
			EnableWindow $download_port 1
		${Else}
			EnableWindow $auth_port 0
			EnableWindow $gss_port 0
			EnableWindow $lss_port 0
			EnableWindow $gds_port 0
			EnableWindow $lds_port 0
			EnableWindow $MongoDBCustomPortField 0
			EnableWindow $admin_port 0
			EnableWindow $download_port 0
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
		${NSD_GetText} $admin_port $admin_port_content
		${NSD_GetText} $download_port $download_port_content
		
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_AUTH_PORT"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GSS_PORT"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LSS_PORT"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GDS_PORT"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LDS_PORT"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_MONGODB_ADDRESS"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_ADMIN_PORT"
		DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_DOWNLOAD_PORT"

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
			StrCpy $admin_entry "8000"
			StrCpy $download_entry "80"

			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_AUTH_PORT" "$auth_entry"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GSS_PORT" "$gss_entry"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LSS_PORT" "$lss_entry"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GDS_PORT" "$gds_entry"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LDS_PORT" "$lds_entry"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_MONGODB_ADDRESS" "$NetworkModeSelection:$MONGODB_CUSTOM_PORT"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_ADMIN_PORT" "$admin_entry"
			WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_DOWNLOAD_PORT" "$download_entry"
		Goto EndCheck
				
		customPortRadioBtnChecked:
			${NSD_GetText} $auth_port $0
			${NSD_GetText} $gss_port $1
			${NSD_GetText} $lss_port $2
			${NSD_GetText} $gds_port $3
			${NSD_GetText} $lds_port $4
			${NSD_GetText} $MongoDBCustomPortField $5
			${NSD_GetText} $admin_port $6
			${NSD_GetText} $download_port $7

			#check for empty fields
			${If} $0 == "" 
			${OrIf} $1 == "" 
			${OrIf} $2 == "" 
			${OrIf} $3 == "" 
			${OrIf} $4 == ""
			${OrIf} $5 == ""
			${OrIf} $6 == ""
			${OrIf} $7 == ""
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
			${OrIf} $6 > 65535
				${OrIf} $6 < 1
			${OrIf} $7 > 65535
				${OrIf} $7 < 1
				MessageBox MB_ICONSTOP|MB_OK "One or more port numbers exceed the valid range of 1 - 65535."
				Abort
			${EndIf}	

			#check for redundant entries
			${If} $0 == $1 
			${OrIf} $0 == $2 
			${OrIf} $0 == $3 
			${OrIf} $0 == $4 
			${OrIf} $0 == $5
			${OrIf} $0 == $6
			${OrIf} $0 == $7

			${OrIf} $1 == $2 
			${OrIf} $1 == $3 
			${OrIf} $1 == $4
			${OrIf} $1 == $5 
			${OrIf} $1 == $6 
			${OrIf} $1 == $7 

			${OrIf} $2 == $3 
			${OrIf} $2 == $4 
			${OrIf} $2 == $5
			${OrIf} $2 == $6
			${OrIf} $2 == $7

			${OrIf} $3 == $4
			${OrIf} $3 == $5
			${OrIf} $3 == $6
			${OrIf} $3 == $7

			${OrIf} $4 == $5
			${OrIf} $4 == $6
			${OrIf} $4 == $7

			${OrIf} $5 == $6
			${OrIf} $5 == $7

			${OrIf} $6 == $7
				MessageBox MB_ICONSTOP|MB_OK "Port values must be unique. Please ensure no two ports are the same"
				Abort
			${EndIf}
				${NSD_GetText} $auth_port $auth_entry
				${NSD_GetText} $gss_port $gss_entry
				${NSD_GetText} $lss_port $lss_entry
				${NSD_GetText} $gds_port $gds_entry
				${NSD_GetText} $lds_port $lds_entry
				${NSD_GetText} $MongoDBCustomPortField $MONGODB_CUSTOM_PORT
				${NSD_GetText} $admin_port $admin_entry
				${NSD_GetText} $download_port $download_entry

				#set new custom env variable values and notify the system every time
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_AUTH_PORT" "$auth_entry"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GSS_PORT" "$gss_entry"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LSS_PORT" "$lss_entry"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GDS_PORT" "$gds_entry"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LDS_PORT" "$lds_entry"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_MONGODB_ADDRESS" "$NetworkModeSelection:$MONGODB_CUSTOM_PORT"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_ADMIN_PORT" "$admin_entry"
				WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_DOWNLOAD_PORT" "$download_entry"
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
			${NSD_CreateLabel} 0 105u 100% 12u $admin_entry
				${NSD_CreateLabel} 55 105u 100% 12u "Admin Panel Port"
			${NSD_CreateLabel} 0 125u 100% 12u $download_entry
				${NSD_CreateLabel} 55 125u 100% 12u "Frontend Download Port"
	
		nsDialogs::Show
	FunctionEnd
*/


; MUI Settings
	
	!define MUI_ABORTWARNING
	!define MUI_ICON ${MUI_ICON_PATH}
	!define MUI_UNICON ${MUI_UNICON_PATH} 

	; Welcome page
	!insertmacro MUI_PAGE_WELCOME
	; License page
	!define MUI_LICENSEPAGE_CHECKBOX
	!insertmacro MUI_PAGE_LICENSE ${LICENSE_FILE_PATH}

	
	Page custom SetupMongoDBInstallation OnSetupMongoDBInstallationLeave
	!define MUI_COMPONENTSPAGE_TEXT_TOP $MONGODB_SETUP_MSG
	!insertmacro MUI_PAGE_COMPONENTS
	
	Page custom DataBaseAccess_Query DataBaseAccess_Leave ;Just in case an upgrade is selected
	
	; Page calls - order is relevant!
	; Directory page
	!insertmacro MUI_PAGE_DIRECTORY
	
	;Necesssary if MongoDB gets installed for the first time
	Page custom DatabaseEntry
	Page custom DatabasePassword OnDatabasePasswordLeave
	Page custom NetworkModePage OnNetworkLeave
	Page custom PublicIPCertificate OnPublicCertficateLeave
	Page custom PortPage OnPortLeave
	
	;If there is already a MongoDB Installation, an Upgrade of some kind is needed
	

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
;OutFile "Install OpenTwin.exe"
InstallDir "$PROGRAMFILES\OpenTwin" ; = $INSTDIR
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails hide
ShowUnInstDetails hide

Section "-Extract Installer Tools (Required)" SEC01
	SectionIn RO ;read only section
	SetOutPath "$INSTDIR\Tools\ThirdParty"
	DetailPrint "Extracting toolchain..."
	File /r "..\..\..\..\ThirdParty\Installer_Tools\ThirdParty\shared\RefreshEnv.cmd"
	File /r "..\..\..\..\ThirdParty\Installer_Tools\ThirdParty\shared\VC_redist.x64.exe"
	File /r "..\..\..\..\ThirdParty\Installer_Tools\ThirdParty\shared\VC_redist.x86.exe"
	
	SetOutPath ${LOG_PATH}
	File "${HELPER_FILES_PATH}\BuildInfo.txt"
	
	DetailPrint "Extracting installation helper..."
	SetOutPath "$INSTDIR"
	File /r "${HELPER_FILES_PATH}\Configuration\*.*"

	DetailPrint "Extracting javascript files..."
	SetOutPath "$INSTDIR\Tools\javascript"
	File /r "..\javascript\*.*"

SectionEnd

Section "OpenTwin Main Files (Required)" SEC02
	SectionIn RO ;read only section
	SetOutPath "$INSTDIR"
	SetOverwrite ifnewer

	DetailPrint "Extracting..."

	File /r "..\..\..\Deployment\*.*"
		#relative to script location
		#    ".\" 	 = the script location itself
		#    "..\" 	 = one directory up
		#    "..\..\"  = two directories up etc.

	${If} $PublicIpSet <> 0 #public IP was set
		ExpandEnvStrings $0 %COMSPEC%
		ExecWait '"$0" /c "START /WAIT /MIN cmd.exe /c ""$INSTDIR\Tools\ThirdParty\RefreshEnv.cmd" && cd "$INSTDIR\Certificates" && "$INSTDIR\Certificates\CreateServerCertificate.bat""" '
	${Else}
		Goto +2
	${EndIf}

	DetailPrint "Installing VC Redistributable..."
	# The 32bit version is required for Apache
	ExecWait '"$INSTDIR\Tools\ThirdParty\VC_redist.x86.exe" /silent'					
	ExecWait '"$INSTDIR\Tools\ThirdParty\VC_redist.x64.exe" /silent'					

	; Shortcuts
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
	CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
	CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\OpenTwin.lnk" "$INSTDIR\OpenTwin_local.bat" "" ${OPENTWIN_APP_ICON}
	CreateShortCut "$DESKTOP\OpenTwin.lnk" "$INSTDIR\OpenTwin_local.bat" "" ${OPENTWIN_APP_ICON}
	!insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

####### Checking if a MongoDB upgrade or a first installation is required ######

#################################################################################
!include ${UPGRADER_ROOT}\Upgrader_NSIS\MongoDBUpgrader.nsh
!include MongoUpgraderTemplateFunctions.nsh
!include CheckForMongoInstallation.nsh
!include MongoFirstInstallSetup.nsh

Section "-Post MongoInstall"
	;MessageBox MB_OK "Deleting Mongo Files"
	DetailPrint "Removing MongoDB executables ..."
	
	RMDir /r "$INSTDIR\Upgrader"
	;RMDir /r "${MONGODB_INST_SERVER_PATH}"
	;RMDir /r "${MONGODB_UPGRADER_INST_EXE_PATH}"
	;RMDir /r "${MONGODB_INST_MSI_PATH}"
 
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
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "Publisher" "http://www.opentwin.net"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayIcon" ${OPENTWIN_APP_ICON}
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "UninstallString" '"$INSTDIR\Uninstall_OpenTwin.exe"'
	WriteRegDWORD HKLM "${REGPATH_UNINSTSUBKEY}" "NoModify" 1
	WriteRegDWORD HKLM "${REGPATH_UNINSTSUBKEY}" "NoRepair" 1

SectionEnd

#Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "Install all required OpenTwin files"
  !insertmacro MUI_DESCRIPTION_TEXT ${Sec_Upgr} "Upgrade of the existing data storage. !WARNING! Once you have upgraded to version 7.0, you will not be able to downgrade the FCV and binary version without support assistance."
  !insertmacro MUI_DESCRIPTION_TEXT ${Sec_Inst} "Install MongoDB 7.0. The configuration from the current installation will be used for the new installation."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function DataBaseAccess_Leave
	Pop $AdminPswHandle
	${NSD_GetText} $AdminPswHandle $UPGRADER_MONGODB_ADMIN_PASSWORD
	;MessageBox MB_OK "Psw. $UPGRADER_MONGODB_ADMIN_PASSWORD"
FunctionEnd

Function DataBaseAccess_Query

	${IfNot} ${SectionIsSelected} ${Sec_Upgr}
		Abort
	${EndIf}
	!insertmacro MUI_HEADER_TEXT "MongoDB Admin Access" "In order to upgrade the existing MongoDB installation, the MongoDB admin psw. is needed. Please enter:"
	nsDialogs::Create 1018
	Pop $Dialog

	${If} $Dialog == error
		Abort
	${EndIf}
		
	${NSD_CreateLabel} 0u 0u 100% 12u "Enter MongoDB admin Psw:"
	Pop $Label
	${NSD_CreateText} 0u 20u 100% 12u "admin"
	Pop $AdminPswHandle
	${NSD_Edit_SetReadOnly} $AdminPswHandle 0
	
	nsDialogs::Show
FunctionEnd


#UNINSTALLER - non functional, unfinished
Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit	
	;MessageBox MB_OK "First uninstall"
  	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +1 IDNO AbortUninstall
  	;MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Do you also want to uninstall MongoDB and all of its components?" IDYES +1 IDNO +3
	;StrCpy $UNINSTALL_MONGODB_FLAG 1
	;	Goto +4
	;StrCpy $UNINSTALL_MONGODB_FLAG 0
	Goto +2
AbortUninstall:
    Abort
 			
	SetRegView 32
  	ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString"
	
	Push $0
	Call un.GetParent
	Pop $R0 ; this is the installation directory
	
	StrCpy $1 "$R0" "" 1
	
	GetFullPathName $3 "$1\ShutdownAll.bat" 
	SetOutPath "$1"
						
	ExecWait '"$3"'
FunctionEnd

#.onInit function to initialize any values that need initializing at the beginning of the script
Function .onInit
	#from front end installer
	!insertmacro EnsureAdminRights
		
	ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString"
	
	${If} $0 != ""
	    MessageBox MB_YESNO|MB_ICONQUESTION "An OpenTwin installation was found. If you continue, the previous version will be removed.$\n$\nDo you really want to proceed?" /SD IDYES IDYES +2
			Abort
			
		Push $0
		Call GetParent
		Pop $R0 ; this is the installation directory
		
		StrCpy $1 "$R0" "" 1

		GetFullPathName $3 "$1\ShutdownAll.bat" 
		SetOutPath "$1"
						
		ExecWait '"$3"'

	    !insertmacro UninstallExisting $0 $0
	    ${If} $0 <> 0
			MessageBox MB_YESNO|MB_ICONSTOP "Failed to uninstall, continue anyway?" /SD IDYES IDYES +2
				Abort
		${EndIf}

	${EndIf}

    StrCpy $PortReturnChecker 0
	StrCpy $PublicIpSet 0
	StrCpy $PublicCertPageChecker 0
	StrCpy $UPGRADER_MONGODB_ADMIN_PASSWORD ${DEFAULT_MONGODB_ADMIN_PASSWORD}

	StrCpy "$ROOTDIR" "$WINDIR" 2
	
	StrCpy $UPGRADER_MONGODB_SERVICE_NAME "MongoDB" ; Default value
	
	# now read the defaults from potentially set environment variables (from a previous installation)
	ReadRegStr $public_ip_field_content HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_SERVICES_ADDRESS"
		
	${If} $public_ip_field_content != ""
		${If} $public_ip_field_content == "127.0.0.1"
			StrCpy $localhost_radio_btn_state ${BST_CHECKED}
		${Else}
			StrCpy $RadioButtonPublic_state ${BST_CHECKED}
		${EndIf}
	${EndIf}
	
	ReadRegStr $MongoDBCustomPortField_content HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_MONGODB_ADDRESS"

	${StrStr} $0 $MongoDBCustomPortField_content ":"
	StrCpy $MongoDBCustomPortField_content $0 "" 1
	
	ReadRegStr $auth_port_content HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_AUTH_PORT"
	ReadRegStr $gss_port_content HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GSS_PORT"
	ReadRegStr $lss_port_content HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LSS_PORT"
	ReadRegStr $gds_port_content HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_GDS_PORT"
	ReadRegStr $lds_port_content HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_LDS_PORT"
	ReadRegStr $admin_port_content HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_ADMIN_PORT"
	ReadRegStr $download_port_content HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_DOWNLOAD_PORT"
	
	${If} $MongoDBCustomPortField_content != ""
	${AndIf} $auth_port_content != ""
	${AndIf} $gss_port_content != ""
	${AndIf} $lss_port_content != ""
	${AndIf} $gds_port_content != ""
	${AndIf} $lds_port_content != ""
	${AndIf} $admin_port_content != ""
	${AndIf} $download_port_content != ""
	    StrCpy $PortReturnChecker 1
		${If} $MongoDBCustomPortField_content != "27017"
		${OrIf} $auth_port_content != "8092"
		${OrIf} $gss_port_content != "8091"
		${OrIf} $lss_port_content != "8093"
		${OrIf} $gds_port_content != "9094"
		${OrIf} $lds_port_content != "9095"
		${OrIf} $admin_port_content != "8000"
		${OrIf} $download_port_content != "80"
			StrCpy $customPortValuesRadioBtnState ${BST_CHECKED}
		${Else}
			StrCpy $standardPortValuesRadioBtnState ${BST_CHECKED}
		${EndIf}
	${EndIf}
	
	ReadRegStr $certs_path_content HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "OPEN_TWIN_CERTS_PATH"
	
	${If} $MongoDBCustomPortField_content != ""
	    StrCpy $PublicCertPageChecker 1
	${EndIf}

FunctionEnd

Section Uninstall
	
	SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000
#===============================================================================================================================	
	
;${If} $UNINSTALL_MONGODB_FLAG != 0
	;nsExec::ExecToLog 'net stop "MongoDB"'
	;ExecWait 'sc delete MongoDB'

	;Delete "$UPGRADER_MONGODB_INSTALL_PATH\*.*"
	;RMDir /r "$UPGRADER_MONGODB_INSTALL_PATH"
	;RMDir /r ${MONGODB_DELETION_PATH}
;${EndIf}


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