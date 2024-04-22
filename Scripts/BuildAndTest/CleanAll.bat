@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 3. DEVENV_ROOT_2022
IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto PAUSE_END
)

IF "%DEVENV_ROOT_2022%" == "" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto PAUSE_END
)

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)


REM ################################################################################################################################
REM LIBRARIES

ECHO ===============================================================
ECHO Clean Library: OpenTwinSystem
ECHO ===============================================================
CALL "%OT_SYSTEM_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: OpenTwinCore
ECHO ===============================================================
CALL "%OT_CORE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: OTRandom
ECHO ===============================================================
CALL "%OT_RANDOM_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: OTGui
ECHO ===============================================================
CALL "%OT_GUI_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: OTGuiAPI
ECHO ===============================================================
CALL "%OT_GUIAPI_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: OTWidgets
ECHO ===============================================================
CALL "%OT_WIDGETS_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: OpenTwinCommunications
ECHO ===============================================================
CALL "%OT_COMMUNICATION_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: DataStorage
ECHO ===============================================================
CALL "%OT_DATASTORAGE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: ModelEntities
ECHO ===============================================================
CALL "%OT_MODELENTITIES_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: CADModelEntities
ECHO ===============================================================
CALL "%OT_CADMODELENTITIES_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: BlockEntities
ECHO ===============================================================
CALL "%OT_BLOCKENTITIES_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: ServiceFoundation
ECHO ===============================================================
CALL "%OT_FOUNDATION_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: StudioSuiteConnector
ECHO ===============================================================
CALL "%OT_STUDIO_SUITE_CONNECTOR_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: uiCore
ECHO ===============================================================
CALL "%OT_UICORE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: UI Plugin API
ECHO ===============================================================
CALL "%OT_UIPLUGINAPI_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: RubberbandEngine: Core
ECHO ===============================================================
CALL "%OT_RUBBERBANDAPI_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: RubberbandEngine: osg Wrapper
ECHO ===============================================================
CALL "%OT_RUBBERBAND_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: QWT Wrapper
ECHO ===============================================================
CALL "%OT_QWTWRAPPER_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: Viewer
ECHO ===============================================================
CALL "%OT_VIEWER_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: Result Data Access
ECHO ===============================================================
CALL "%OT_RESULT_DATA_ACCESS_ROOT%\Clean.bat" 

REM ################################################################################################################################
REM SERVICES

ECHO ===============================================================
ECHO Clean Service: Model
ECHO ===============================================================
CALL "%OT_MODEL_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: GlobalSessionService
ECHO ===============================================================
CALL "%OT_GLOBAL_SESSION_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: LocalSessionService
ECHO ===============================================================
CALL "%OT_LOCAL_SESSION_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: GlobalDirectoryService
ECHO ===============================================================
CALL "%OT_GLOBAL_DIRECTORY_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: LocalDirectoryService
ECHO ===============================================================
CALL "%OT_LOCAL_DIRECTORY_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: RelayService
ECHO ===============================================================
CALL "%OT_RELAY_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: LoggerService
ECHO ===============================================================
CALL "%OT_LOGGER_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: AuthorisationService
ECHO ===============================================================
CALL "%OT_AUTHORISATION_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: PHREECService
ECHO ===============================================================
CALL "%OT_PHREEC_SERVICE_ROOT%\Clean.bat" 

REM ECHO ===============================================================
REM ECHO Clean Service: KrigingService
REM ECHO ===============================================================
REM CALL "%OT_KRIGING_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: ModelingService
ECHO ===============================================================
CALL "%OT_MODELING_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: VisualizationService
ECHO ===============================================================
CALL "%OT_VISUALIZATION_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: FITTDService
ECHO ===============================================================
CALL "%OT_FITTD_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: CartesianMeshService
ECHO ===============================================================
CALL  "%OT_CARTESIAN_MESH_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: TetMeshService
ECHO ===============================================================
CALL  "%OT_TET_MESH_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: ImportParameterizedData
ECHO ===============================================================
CALL "%OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: GetDPService
ECHO ===============================================================
CALL "%OT_GETDP_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: ElmerFEMService
ECHO ===============================================================
CALL "%OT_ELMERFEM_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: StudioSuiteService
ECHO ===============================================================
CALL "%OT_STUDIOSUITE_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: PythonExecutionService
ECHO ===============================================================
CALL "%OT_PYTHON_EXECUTION_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: PythonExecution
ECHO ===============================================================
CALL "%OT_PYTHON_EXECUTION_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: BlockEditorService
ECHO ===============================================================
CALL "%OT_BLOCKEDITORSERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: DataProcessingService
ECHO ===============================================================
CALL "%OT_DATA_PROCESSING_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Service: CircuitSimulatorService
ECHO ===============================================================
CALL "%OT_CIRCUIT_SIMULATOR_SERVICE_ROOT%\Clean.bat" 

REM ################################################################################################################################
REM EXECUTABLES

ECHO ===============================================================
ECHO Clean Executable: uiFrontend
ECHO ===============================================================
CALL "%OT_UI_SERVICE_ROOT%\Clean.bat" 

ECHO ===============================================================
ECHO Clean Library: OToolkitAPI
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\OToolkitAPI\Clean.bat" 

ECHO ===============================================================
ECHO Clean Executable: OToolkit
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\OToolkit\Clean.bat" 

GOTO END

:PAUSE_END
pause
GOTO END

:END
