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

REM ====================================================================
REM Setup eviroment, shutdown services and delete existing libraries
REM ====================================================================

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)

IF "%OPENTWIN_DEPLOYMENT_DIR%" == "" (
    SET OPENTWIN_DEPLOYMENT_DIR=%OPENTWIN_DEV_ROOT%\Deployment
)


CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\ShutdownAll.bat"


REM ====================================================================
REM First, build with special setting Release 
REM ====================================================================

SET OT_RELEASE_DEBUG=1

CALL BuildAll.bat


ECHO Delete libraries

SET OPENTWIN_DEBUG_FILES=%OPENTWIN_DEV_ROOT%\DebugFiles

mkdir %OPENTWIN_DEBUG_FILES% 2>nul

DEL "%OPENTWIN_DEBUG_FILES%\BlockEditorService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\OTSystem.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\OTCore.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\OTRandom.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\OTCommunication.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\OTServiceFoundation.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\ModelEntities.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\CADModelEntities.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\BlockEntities.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\Model.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\Viewer.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\DataStorage.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\uiCore.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\RubberbandEngineCore.dll" 2>NUL	
DEL "%OPENTWIN_DEBUG_FILES%\RubberbandEngineOsgWrapper.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\QwtWrapper.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\GlobalSessionService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\LocalSessionService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\GlobalDirectoryService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\LocalDirectoryService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\uiFrontend.exe" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\RelayService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\LoggerService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\AuthorisationService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\PHREECService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\ModelingService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\VisualizationService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\FITTDService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\PythonExecutionService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\PythonExecution.exe" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\ImportParameterizedDataService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\DataProcessingService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\CartesianMeshService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\TetMeshService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\GetDPService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\ElmerFEMService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\StudioSuiteService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\LTSpiceService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\OTGui.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\OTGuiAPI.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\OTWidgets.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\StudioSuiteConnector.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\LTSpiceConnector.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\open_twin.exe" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\CircuitSimulatorService.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\CircuitExecution.exe" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\OToolkit.dll" 2>NUL
DEL "%OPENTWIN_DEBUG_FILES%\ResultDataAccess.dll" 2>NUL

REM ====================================================================
REM  Copy libraries
REM ====================================================================

ECHO Copy libraries

COPY "%OT_CORE_ROOT%\%OT_DLLR%\OTCore.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_RANDOM_ROOT%\%OT_DLLR%\OTRandom.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_SYSTEM_ROOT%\%OT_DLLR%\OTSystem.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_COMMUNICATION_ROOT%\%OT_DLLR%\OTCommunication.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_FOUNDATION_ROOT%\%OT_DLLR%\OTServiceFoundation.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_MODELENTITIES_ROOT%\%OT_DLLR%\ModelEntities.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_CADMODELENTITIES_ROOT%\%OT_DLLR%\CADModelEntities.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_VIEWER_ROOT%\%OT_DLLR%\Viewer.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_DATASTORAGE_ROOT%\%OT_DLLR%\DataStorage.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_UICORE_ROOT%\%OT_DLLR%\uiCore.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_RUBBERBANDAPI_ROOT%\%OT_DLLR%\RubberbandEngineCore.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_RUBBERBAND_ROOT%\%OT_DLLR%\RubberbandEngineOsgWrapper.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_QWTWRAPPER_ROOT%\%OT_DLLR%\QwtWrapper.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_GUI_ROOT%\%OT_DLLR%\OTGui.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_GUIAPI_ROOT%\%OT_DLLR%\OTGuiAPI.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_WIDGETS_ROOT%\%OT_DLLR%\OTWidgets.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_STUDIO_SUITE_CONNECTOR_ROOT%\%OT_DLLR%\StudioSuiteConnector.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_LTSPICE_CONNECTOR_ROOT%\%OT_DLLR%\LTSpiceConnector.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_BLOCKENTITIES_ROOT%\%OT_DLLR%\BlockEntities.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_RESULT_DATA_ACCESS_ROOT%\%OT_DLLR%\ResultDataAccess.dll" "%OPENTWIN_DEBUG_FILES%"

REM ====================================================================
REM  Copy Services
REM ====================================================================

ECHO Copy Services

COPY "%OT_MODEL_SERVICE_ROOT%\%OT_DLLR%\Model.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_GLOBAL_SESSION_SERVICE_ROOT%\%OT_DLLR%\GlobalSessionService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_LOCAL_SESSION_SERVICE_ROOT%\%OT_DLLR%\LocalSessionService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_GLOBAL_DIRECTORY_SERVICE_ROOT%\%OT_DLLR%\GlobalDirectoryService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_LOCAL_DIRECTORY_SERVICE_ROOT%\%OT_DLLR%\LocalDirectoryService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_UI_SERVICE_ROOT%\%OT_DLLR%\uiFrontend.exe" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_RELAY_SERVICE_ROOT%\%OT_DLLR%\RelayService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_LOGGER_SERVICE_ROOT%\%OT_DLLR%\LoggerService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_AUTHORISATION_SERVICE_ROOT%\%OT_DLLR%\AuthorisationService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_PHREEC_SERVICE_ROOT%\%OT_DLLR%\PHREECService.dll" "%OPENTWIN_DEBUG_FILES%"
REM COPY "%OT_KRIGING_SERVICE_ROOT%\%OT_DLLR%\KrigingService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_MODELING_SERVICE_ROOT%\%OT_DLLR%\ModelingService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_VISUALIZATION_SERVICE_ROOT%\%OT_DLLR%\VisualizationService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_FITTD_SERVICE_ROOT%\%OT_DLLR%\FITTDService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT%\%OT_DLLR%\ImportParameterizedDataService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_CARTESIAN_MESH_SERVICE_ROOT%\%OT_DLLR%\CartesianMeshService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_TET_MESH_SERVICE_ROOT%\%OT_DLLR%\TetMeshService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_GETDP_SERVICE_ROOT%\%OT_DLLR%\GetDPService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_ELMERFEM_SERVICE_ROOT%\%OT_DLLR%\ElmerFEMService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_STUDIOSUITE_SERVICE_ROOT%\%OT_DLLR%\StudioSuiteService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_LTSPICE_SERVICE_ROOT%\%OT_DLLR%\LTSpiceService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_PYTHON_EXECUTION_SERVICE_ROOT%\%OT_DLLR%\PythonExecutionService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_PYTHON_EXECUTION_ROOT%\%OT_DLLR%\PythonExecution.exe" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_BLOCKEDITORSERVICE_ROOT%\%OT_DLLR%\BlockEditorService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_DATA_PROCESSING_SERVICE_ROOT%\%OT_DLLR%\DataProcessingService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_CIRCUIT_SIMULATOR_SERVICE_ROOT%\%OT_DLLR%\CircuitSimulatorService.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OT_CIRCUIT_EXECUTION_ROOT%\%OT_DLLR%\CircuitExecution.exe" "%OPENTWIN_DEBUG_FILES%"

COPY "%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\target\debug\open_twin.exe" "%OPENTWIN_DEBUG_FILES%"

REM ====================================================================
REM  Copy Tools
REM ====================================================================

ECHO Copy Tools
COPY "%OPENTWIN_DEV_ROOT%\Tools\OToolkitAPI\x64\Release\OToolkitAPI.dll" "%OPENTWIN_DEBUG_FILES%"
COPY "%OPENTWIN_DEV_ROOT%\Tools\OToolkit\x64\Release\OToolkit.dll" "%OPENTWIN_DEBUG_FILES%"

GOTO END

:PAUSE_END
pause
GOTO END

:END
