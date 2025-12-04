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

ECHO Delete libraries

DEL "%OPENTWIN_DEPLOYMENT_DIR%\DebugService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTSystem.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTCore.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTRandom.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTCommunication.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTServiceFoundation.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\ModelEntities.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\CADModelEntities.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\BlockEntities.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\Model.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTModelAPI.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\Viewer.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\DataStorage.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\uiCore.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\RubberbandEngineCore.dll" 2>NUL	
DEL "%OPENTWIN_DEPLOYMENT_DIR%\RubberbandEngineOsgWrapper.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\GlobalSessionService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\LocalSessionService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\GlobalDirectoryService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\LocalDirectoryService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\uiFrontend.exe" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\RelayService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\LoggerService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\AuthorisationService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\PHREECService.dll" 2>NUL
REM DEL "%OPENTWIN_DEPLOYMENT_DIR%\KrigingService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\ModelingService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\VisualizationService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\FITTDService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\PythonExecutionService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\PythonExecution.exe" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\ImportParameterizedDataService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\DataProcessingService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\CartesianMeshService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\TetMeshService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\GetDPService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\FDTDService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\ElmerFEMService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\StudioSuiteService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\LTSpiceService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\PyritService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTGui.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTGuiAPI.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTWidgets.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTFrontendConnectorAPI.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTFMC.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\StudioSuiteConnector.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\LTSpiceConnector.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\open_twin.exe" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\CircuitSimulatorService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\CircuitExecution.exe" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OToolkit.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\ResultDataAccess.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTSystemInformationTool.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\ModelLibraryUpdater.exe" 2>Nul
DEL "%OPENTWIN_DEPLOYMENT_DIR%\LibraryManagementService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\HierarchicalProjectService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\FileManagementProjectService.dll" 2>NUL

ECHO Delete color styles
DEL "%OPENTWIN_DEPLOYMENT_DIR%\ColorStyles\*.otcsf" 2>NUL

ECHO Delete GraphicsItems
RMDIR /S /Q "%OPENTWIN_DEPLOYMENT_DIR%\GraphicsItems" 2>NUL

ECHO Delete Admin Panel 

MKDIR "%OPENTWIN_DEPLOYMENT_DIR%\Apache" 2>NUL
RMDIR /Q /S "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"
MKDIR "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"

REM ====================================================================
REM  Copy libraries
REM ====================================================================

ECHO Copy libraries

COPY "%OT_CORE_ROOT%\%OT_CDLLR%\OTCore.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_RANDOM_ROOT%\%OT_CDLLR%\OTRandom.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_SYSTEM_ROOT%\%OT_CDLLR%\OTSystem.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_COMMUNICATION_ROOT%\%OT_CDLLR%\OTCommunication.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_FOUNDATION_ROOT%\%OT_DLLR%\OTServiceFoundation.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_MODELENTITIES_ROOT%\%OT_CDLLR%\ModelEntities.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_CADMODELENTITIES_ROOT%\%OT_DLLR%\CADModelEntities.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_VIEWER_ROOT%\%OT_DLLR%\Viewer.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_DATASTORAGE_ROOT%\%OT_CDLLR%\DataStorage.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_UICORE_ROOT%\%OT_DLLR%\uiCore.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_RUBBERBANDAPI_ROOT%\%OT_DLLR%\RubberbandEngineCore.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_RUBBERBAND_ROOT%\%OT_DLLR%\RubberbandEngineOsgWrapper.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_GUI_ROOT%\%OT_CDLLR%\OTGui.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_GUIAPI_ROOT%\%OT_CDLLR%\OTGuiAPI.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_MODELAPI_ROOT%\%OT_DLLR%\OTModelAPI.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_WIDGETS_ROOT%\%OT_DLLR%\OTWidgets.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_FRONTEND_CONNECTOR_API_ROOT%\%OT_DLLR%\OTFrontendConnectorAPI.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_FILE_MANAGER_CONNECTOR_ROOT%\%OT_DLLR%\OTFMC.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_STUDIO_SUITE_CONNECTOR_ROOT%\%OT_DLLR%\StudioSuiteConnector.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_LTSPICE_CONNECTOR_ROOT%\%OT_DLLR%\LTSpiceConnector.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_BLOCKENTITIES_ROOT%\%OT_DLLR%\BlockEntities.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_RESULT_DATA_ACCESS_ROOT%\%OT_DLLR%\ResultDataAccess.dll" "%OPENTWIN_DEPLOYMENT_DIR%"

REM ====================================================================
REM  Copy Services
REM ====================================================================

ECHO Copy Services

COPY "%OT_MODEL_SERVICE_ROOT%\%OT_DLLR%\Model.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_GLOBAL_SESSION_SERVICE_ROOT%\%OT_DLLR%\GlobalSessionService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_LOCAL_SESSION_SERVICE_ROOT%\%OT_DLLR%\LocalSessionService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_GLOBAL_DIRECTORY_SERVICE_ROOT%\%OT_DLLR%\GlobalDirectoryService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_LOCAL_DIRECTORY_SERVICE_ROOT%\%OT_DLLR%\LocalDirectoryService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_UI_SERVICE_ROOT%\%OT_DLLR%\uiFrontend.exe" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_RELAY_SERVICE_ROOT%\%OT_DLLR%\RelayService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_LOGGER_SERVICE_ROOT%\%OT_DLLR%\LoggerService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_AUTHORISATION_SERVICE_ROOT%\%OT_DLLR%\AuthorisationService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_PHREEC_SERVICE_ROOT%\%OT_DLLR%\PHREECService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
REM COPY "%OT_KRIGING_SERVICE_ROOT%\%OT_DLLR%\KrigingService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_MODELING_SERVICE_ROOT%\%OT_DLLR%\ModelingService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_VISUALIZATION_SERVICE_ROOT%\%OT_DLLR%\VisualizationService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_FITTD_SERVICE_ROOT%\%OT_DLLR%\FITTDService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT%\%OT_DLLR%\ImportParameterizedDataService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_CARTESIAN_MESH_SERVICE_ROOT%\%OT_DLLR%\CartesianMeshService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_TET_MESH_SERVICE_ROOT%\%OT_DLLR%\TetMeshService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_GETDP_SERVICE_ROOT%\%OT_DLLR%\GetDPService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_FDTD_SERVICE_ROOT%\%OT_DLLR%\FDTDService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_ELMERFEM_SERVICE_ROOT%\%OT_DLLR%\ElmerFEMService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_STUDIOSUITE_SERVICE_ROOT%\%OT_DLLR%\StudioSuiteService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_LTSPICE_SERVICE_ROOT%\%OT_DLLR%\LTSpiceService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_PYRIT_SERVICE_ROOT%\%OT_DLLR%\PyritService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_PYTHON_EXECUTION_SERVICE_ROOT%\%OT_DLLR%\PythonExecutionService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_PYTHON_EXECUTION_ROOT%\%OT_DLLR%\PythonExecution.exe" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_DEBUGSERVICE_ROOT%\%OT_DLLR%\DebugService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_DATA_PROCESSING_SERVICE_ROOT%\%OT_DLLR%\DataProcessingService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_CIRCUIT_SIMULATOR_SERVICE_ROOT%\%OT_DLLR%\CircuitSimulatorService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_CIRCUIT_EXECUTION_ROOT%\%OT_DLLR%\CircuitExecution.exe" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_LIBRARY_MANAGEMENT_SERVICE_ROOT%\%OT_DLLR%\LibraryManagementService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_HIERARCHICAL_PROJECT_SERVICE_ROOT%\%OT_DLLR%\HierarchicalProjectService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_FILEMANAGEMENT_PROJECT_SERVICE_ROOT%\%OT_DLLR%\FileManagementProjectService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"

COPY "%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\target\release\open_twin.exe" "%OPENTWIN_DEPLOYMENT_DIR%"

REM ====================================================================
REM  Copy Tools
REM ====================================================================

ECHO Copy Tools
COPY "%OPENTWIN_DEV_ROOT%\Tools\OToolkitAPI\x64\Release\OToolkitAPI.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Tools\OToolkit\x64\Release\OToolkit.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Tools\OTSystemInformationTool\x64\Release\OTSystemInformationTool.exe" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Tools\ModelLibraryUpdater\x64\Release\ModelLibraryUpdater.exe" "%OPENTWIN_DEPLOYMENT_DIR%"

REM ====================================================================
REM  Copy Color Styles
REM ====================================================================

ECHO Copy ColorStyles
COPY /Y "%OPENTWIN_DEV_ROOT%\Assets\ColorStyles\*.otcsf" "%OPENTWIN_DEPLOYMENT_DIR%\ColorStyles"

REM ====================================================================
REM  Copy GraphicsItems
REM ====================================================================
MKDIR "%OPENTWIN_DEPLOYMENT_DIR%\GraphicsItems"
XCOPY /S "%OPENTWIN_DEV_ROOT%\Assets\GraphicsItems\*.ot.json" "%OPENTWIN_DEPLOYMENT_DIR%\GraphicsItems"

REM ====================================================================
REM  Copy Admin Panel
REM ====================================================================

ECHO Copy Admin Panel
XCOPY /E /Q "%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\build\*.*" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"
COPY "%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\Apache_config\.htaccess" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"
COPY /Y "%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\Apache_config\httpd.conf" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\conf"
COPY /Y "%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\Apache_config\httpd-ahssl.conf" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\conf\extra"

REM ====================================================================
REM  Create the build informatiomn file
REM ====================================================================

CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\CreateBuildInformation.bat"


GOTO END

:PAUSE_END
pause
GOTO END

:END
