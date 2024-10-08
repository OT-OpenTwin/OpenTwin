Function Upgrader_Failed
	MessageBox MB_OK "Failed to determine the current MongoDB version: $1"
	Quit
FunctionEnd

Function Upgrader_Upgrade_Required
	StrCpy $MONGODB_SETUP_MSG \
	"An installed MongoDB version $1 has been detected. OpenTwin runs on MongoDB version $MONGODB_MAX_VERSION and an upgrade is recommended. Please note that an upgrade of existing data may not be reversible without assistance from the MongoDB support group."
	SectionSetFlags ${Sec_Upgr} ${SF_PSELECTED}
	SectionSetFlags ${Sec_Inst} ${SF_PSELECTED}
FunctionEnd

Function Upgrader_Upgrade_NOT_Required
	StrCpy $MONGODB_SETUP_MSG \
	"An installed MongoDB version was detected but no data was found in the database path. OpenTwin now works with MongoDB version $MONGODB_MAX_VERSION and an upgrade is recommended." 
	SectionSetText ${Sec_Upgr} "" ;Upgrade is not required, the new MongoDB version can be installed directly.
	SectionSetFlags ${Sec_Upgr} ${SF_RO}
	
	SectionSetText ${Sec_Mongo_Grp} "" 
	SectionSetFlags ${Sec_Mongo_Grp} ${SF_RO}
FunctionEnd

Function Upgrader_MongoDB_Up_To_Date
	SectionSetText ${Sec_Inst} "" ;Upgrade is not required, the new MongoDB version can be installed directly.
	SectionSetFlags ${Sec_Inst} ${SF_RO}
	SectionSetText ${Sec_Upgr} "" 
	SectionSetFlags ${Sec_Upgr} ${SF_RO}
	SectionSetText ${Sec_Mongo_Grp} "" 
	SectionSetFlags ${Sec_Mongo_Grp} ${SF_RO}
	
	StrCpy $MONGODB_SETUP_MSG \
	"Detected an already installed MongoDB version $1, which is the latest version supported by this installer."
FunctionEnd