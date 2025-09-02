@echo off

REM This script can be used in the following two ways:
REM 1) Regular:   Run the script, this will use the current directory as the root for the services.
REM 2) Developer: Pass the "dev" argument when launching the script, this will use the OpenTwin environment to set up the LDS.
REM In both cases this script will automatically skip the execution if the OT_LOCALDIRECTORYSERVICE_CONFIGURATION_DEFINED is already set.
REM Otherwise the specified configuration will be set (later in the script).

REM Dont set twice / dont overwrite
if defined OT_LOCALDIRECTORYSERVICE_CONFIGURATION_DEFINED (
	goto END
)

REM Check if a start argument was provided
if "%1" == "dev" (
	goto CHECK_DEV_ENV
)
set "OT_BATCH_TMP=%~dp0"
goto FINISH_BATCH_TMP_SETUP


REM Check development environment
:CHECK_DEV_ENV
if "%OPENTWIN_DEV_ROOT%" == "" (
	REM If dev is used, the dev env must be set
	echo Please specify the following environmentvariable when using the "dev" argument for the set up LDS batch: OPENTWIN_DEV_ROOT
	goto PAUSE_END
) else (
	REM In the release mode the services from the deployment will be used
	set OT_BATCH_TMP=%OPENTWIN_DEV_ROOT%\Deployment
)

:FINISH_BATCH_TMP_SETUP

REM Tidy string
set OT_BATCH_TMP=%OT_BATCH_TMP:\=\\%

REM Set config
set OT_LOCALDIRECTORYSERVICE_CONFIGURATION={"DefaultMaxCrashRestarts": 8,^
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
		"FDTDService",^
		"ElmerFEMService",^
		"DataProcessingService",^
		"DebugService",^
		"CircuitSimulatorService",^
		"StudioSuiteService",^
		"LTSpiceService",^
		"PyritService"^
		]^
	}
set OT_LOCALDIRECTORYSERVICE_CONFIGURATION_DEFINED=1

goto END

:PAUSE_END
pause
goto END

:END
