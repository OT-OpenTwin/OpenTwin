@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

Rem Dont set twice / dont overwrite
IF DEFINED OT_LOCALDIRECTORYSERVICE_CONFIGURATION (
	goto END
)

SET OT_BATCH_TMP=%OPENTWIN_DEV_ROOT%
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
		"PythonExecutionService",^
		"GetDPService"^
		]^
	}

GOTO END

:PAUSE_END
pause
GOTO END

:END
