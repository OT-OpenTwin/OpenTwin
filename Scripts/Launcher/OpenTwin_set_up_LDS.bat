@echo off

:: This script can be used in the following two ways:
:: 1) Regular:   Run the script, this will use the current directory as the root for the services.
:: 2) Developer: Pass the "dev" argument when launching the script, this will use the OpenTwin environment to set up the LDS.
:: In both cases this script will automatically skip the execution if the OT_LOCALDIRECTORYSERVICE_CONFIGURATION_DEFINED is already set.
:: Otherwise the specified configuration will be set (later in the script).

:: Dont set twice / dont overwrite
if defined OT_LOCALDIRECTORYSERVICE_CONFIGURATION_DEFINED (
	goto END
)

:: Check if a start argument was provided
if "%1" == "dev" (
	goto CHECK_DEV_ENV
)
set "OT_BATCH_TMP=%~dp0"
goto FINISH_BATCH_TMP_SETUP


:: Check development environment
:CHECK_DEV_ENV
if "%OPENTWIN_DEV_ROOT%" == "" (
	:: If dev is used, the dev env must be set
	echo Please specify the following environmentvariable when using the "dev" argument for the set up LDS batch: OPENTWIN_DEV_ROOT
	goto PAUSE_END
) else (
	:: In the release mode the services from the deployment will be used
	set OT_BATCH_TMP=%OPENTWIN_DEV_ROOT%\Deployment
)

:FINISH_BATCH_TMP_SETUP

:: Tidy string
set OT_BATCH_TMP=%OT_BATCH_TMP:\=\\%

:: Set config
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
		"ElmerFEMService",^
		"DataProcessingService",^
		"BlockEditorService",^
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
