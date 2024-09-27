!include "MUI.nsh"
!include "nsDialogs.nsh"

!include LogicLib.nsh
!include FileFunc.nsh

Name "OpenTwin MongoDB Upgrade"
;Outfile "MongoDBUpgrade_Only.exe"

RequestExecutionLevel admin

################################ Variables ###############################
Var MONGODB_SETUP_MSG
Var MONGODB_MAX_VERSION
Var Dialog
Var Label
Var AdminPswHandle
Var ServiceNameHandle
Var InstallPathHandle


!define DEFAULT_MONGODB_PATH '"C:\Program Files\MongoDB\Server\7.0"'
!define MUI_ICON_PATH '"..\Icons\openTwin_icon_48x48.ico"'
;!define OPENTWIN_APP_ICON '"$INSTDIR\icons\Application\OpenTwin.ico"'
!define PRODUCT_NAME "OpenTwin"
BrandingText "OpenTwin Simulation Platform"

######################## Including Upgrader Functionality ################
!include MongoDBUpgrader.nsh
##########################################################################

##########################################################################
################################ Functions ###############################

Function BeforeUpgrade
	!insertmacro MUI_HEADER_TEXT "Information about running MongoDB installation" "Software for upgrading MongoDB. Please verify the following information:" 
	;MessageBox MB_OK "Entry"
	nsDialogs::Create 1018
	Pop $Dialog
	${If} $Dialog == error
		;MessageBox MB_OK "Fail"
		Abort
	${EndIf}

	${NSD_CreateLabel} 0u 0u 100% 12u "Enter MongoDB admin Psw:"
	Pop $Label
	${NSD_CreateText} 0u 12u 100% 12u "admin"
	Pop $AdminPswHandle
	${NSD_GetText} $AdminPswHandle $UPGRADER_MONGODB_ADMIN_PASSWORD
	
	${NSD_CreateLabel} 0u 30u 100% 12u "Enter MongoDB service name:"
	Pop $Label
	${NSD_CreateText} 0u 42u 100% 12u "MongoDB"
	Pop $ServiceNameHandle
	${NSD_GetText} $ServiceNameHandle $UPGRADER_MONGODB_SERVICE_NAME
	
	${NSD_CreateLabel} 0u 60u 100% 12u "MongoDB Installation path:"
	${NSD_CreateText} 0u 72u 100% 12u ${DEFAULT_MONGODB_PATH}
	Pop $InstallPathHandle
	${NSD_GetText} $InstallPathHandle $UPGRADER_MONGODB_INSTALL_PATH
  
  nsDialogs::Show
  
FunctionEnd


Function Upgrader_Failed
	MessageBox MB_OK "Failed to determine the current MongoDB version: $1"
	Quit
FunctionEnd

Function Upgrader_Upgrade_Required
	StrCpy $MONGODB_SETUP_MSG \
	"An installed MongoDB version $1 has been detected. OpenTwin is now running on MongoDB version $MONGODB_MAX_VERSION and an upgrade is recommanded. Please note that an upgrade of existing data is required and may not be reversible without assistance from the MongoDB support group."
	SectionSetFlags ${Sec_Upgr} ${SF_PSELECTED}
	SectionSetFlags ${Sec_Inst} ${SF_PSELECTED}
FunctionEnd

Function Upgrader_Upgrade_NOT_Required
	StrCpy $MONGODB_SETUP_MSG \
	"An installed MongoDB version was detected but no data was found in the database path. OpenTwin is now works with MongoDB version $MONGODB_MAX_VERSION and an upgrade is recommended." 
	SectionSetText ${Sec_Upgr} "" ;Upgrade is not required, the new MongoDB version can be installed directly.
	SectionSetFlags ${Sec_Upgr} ${SF_RO}
FunctionEnd

Function Upgrader_MongoDB_Up_To_Date
	;Nothing needs to be done.
	MessageBox MB_OK \
	"Detected MongoDB version $1 which is the latest version supported by this installer."
	Quit
FunctionEnd

##########################################################################

!define MUI_ABORTWARNING
;!define MUI_ICON "..\Icons\openTwin_icon_48x48.ico"
!define MUI_ICON ${MUI_ICON_PATH}


Page custom BeforeUpgrade

!define MUI_PAGE_CUSTOMFUNCTION_PRE Upgrader_On_Init
!define MUI_COMPONENTSPAGE_TEXT_TOP $MONGODB_SETUP_MSG
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES 
!insertmacro MUI_LANGUAGE "English"


Function .onInit

FunctionEnd