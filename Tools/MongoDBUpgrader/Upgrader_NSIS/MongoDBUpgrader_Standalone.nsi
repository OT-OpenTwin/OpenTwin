!include "MUI2.nsh"

Name "MongoDB Upgrade"
;Outfile "MongoDBUpgrade_Only.exe"

RequestExecutionLevel admin

!insertmacro MUI_LANGUAGE "English"

################################ Variables ###############################
Var MONGODB_SERVICE_NAME
Var MONGODB_ADMIN_PASSWORD
Var MONGODB_SETUP_MSG
Var MONGODB_MAX_VERSION
Var Dialog
Var Label
Var Output

######################## Including Upgrader Functionality ################
!include MongoDBUpgrader.nsh
##########################################################################

##########################################################################
################################ Functions ###############################

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

##########################################################################


!define MUI_COMPONENTSPAGE_TEXT_TOP $MONGODB_SETUP_MSG
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES 


Function .onInit
	Call Upgrader_On_Init
	
FunctionEnd