@ECHO OFF

SET OT_BATCH_TMP=%SIM_PLAT_ROOT%
SET OT_BATCH_TMP=%OT_BATCH_TMP:\=\\%

SET OT_LOCALDIRECTORYSERVICE_CONFIGURATION={"DefaultMaxCrashRestarts": 8,^
    "DefaultMaxStartupRestarts": 64,^
    "ServicesLibraryPath": "%OT_BATCH_TMP%\\Deployment",^
    "LauncherPath": "%OT_BATCH_TMP%\\Deployment\\open_twin.exe",^
	"SupportedServices": [^
		"CartesianMeshService",^
		"FITTDService",^
		"KrigingService",^
		"Model",^
		"ModelingService",^
		"PHREECService",^
		"RelayService",^
		"TetMeshService",^
		"ImportParameterizedDataService",^
		"VisualizationService",^
		"GetDPService"^
		]^
	}