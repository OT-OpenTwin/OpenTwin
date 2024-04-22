@ECHO OFF

Rem Dont set twice / dont overwrite
IF DEFINED OT_LOCALDIRECTORYSERVICE_CONFIGURATION (
	goto END
)

IF "%OPENTWIN_DEV_ROOT%" == "" (
	SET "OT_BATCH_TMP=%cd%"
) ELSE (
	SET OT_BATCH_TMP=%OPENTWIN_DEV_ROOT%\Deployment
)

SET OT_BATCH_TMP=%OT_BATCH_TMP:\=\\%

SET OT_LOCALDIRECTORYSERVICE_CONFIGURATION={"DefaultMaxCrashRestarts": 8,^
    "DefaultMaxStartupRestarts": 64,^
    "ServicesLibraryPath": "%OT_BATCH_TMP%",^
    "LauncherPath": "%OT_BATCH_TMP%\\open_twin.exe",^
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
		"PythonExecutionService",^
		"GetDPService",^
		"ElmerFEMService",^
		"DataProcessingService",^
		"BlockEditorService",^
		"CircuitSimulatorService",^
		"StudioSuiteService"^
		]^
	}

GOTO END

:PAUSE_END
pause
GOTO END

:END
