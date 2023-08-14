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

REM DEL "%OPENTWIN_DEPLOYMENT_DIR%\BlockEditor.dll"
REM DEL "%OPENTWIN_DEPLOYMENT_DIR%\BlockEditorAPI.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\BlockEditorService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OpenTwinSystem.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OpenTwinCore.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OpenTwinCommunication.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OpenTwinServiceFoundation.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\ModelEntities.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\CADModelEntities.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\Model.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\Viewer.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\DataStorage.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\UIPluginAPI.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\uiCore.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\RubberbandEngineCore.dll"	
DEL "%OPENTWIN_DEPLOYMENT_DIR%\RubberbandEngineOsgWrapper.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\QwtWrapper.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\GlobalSessionService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\SessionService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\GlobalDirectoryService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\LocalDirectoryService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\uiFrontend.exe"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\RelayService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\LoggerService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\AuthorisationService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\PHREECService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\KrigingService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\ModelingService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\VisualizationService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\FITTDService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\PythonExecutionService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\PythonExecution.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\ImportParameterizedDataService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\DataProcessingService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\CartesianMeshService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\TetMeshService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\GetDPService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTGui.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTQtWrapper.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTsci.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTWidgets.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\open_twin.exe"

DEL "%OPENTWIN_DEPLOYMENT_DIR%\OToolkit.dll"

ECHO Delete Admin Panel 

RMDIR /Q /S "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"
MKDIR "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"

REM ====================================================================
REM  Copy libraries
REM ====================================================================

ECHO Copy libraries

REM COPY "%OT_BLOCKEDITOR_ROOT%\%OT_DLLR%\BlockEditor.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
REM COPY "%OT_BLOCKEDITORAPI_ROOT%\%OT_DLLR%\BlockEditorAPI.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_CORE_ROOT%\%OT_DLLR%\OpenTwinCore.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_SYSTEM_ROOT%\%OT_DLLR%\OpenTwinSystem.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_COMMUNICATION_ROOT%\%OT_DLLR%\OpenTwinCommunication.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_FOUNDATION_ROOT%\%OT_DLLR%\OpenTwinServiceFoundation.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_MODELENTITIES_ROOT%\%OT_DLLR%\ModelEntities.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_CADMODELENTITIES_ROOT%\%OT_DLLR%\CADModelEntities.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_VIEWER_ROOT%\%OT_DLLR%\Viewer.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_DATASTORAGE_ROOT%\%OT_DLLR%\DataStorage.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_UIPLUGINAPI_ROOT%\%OT_DLLR%\UIPluginAPI.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_UICORE_ROOT%\%OT_DLLR%\uiCore.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_RUBBERBANDAPI_ROOT%\%OT_DLLR%\RubberbandEngineCore.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_RUBBERBAND_ROOT%\%OT_DLLR%\RubberbandEngineOsgWrapper.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_QWTWRAPPER_ROOT%\%OT_DLLR%\QwtWrapper.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_GUI_ROOT%\%OT_DLLR%\OTGui.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_QTWRAPPER_ROOT%\%OT_DLLR%\OTQtWrapper.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_SCI_ROOT%\%OT_DLLR%\OTsci.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_WIDGETS_ROOT%\%OT_DLLR%\OTWidgets.dll" "%OPENTWIN_DEPLOYMENT_DIR%"

REM ====================================================================
REM  Copy Services
REM ====================================================================

ECHO Copy Services

COPY "%OT_MODEL_SERVICE_ROOT%\%OT_DLLR%\Model.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_GLOBAL_SESSION_SERVICE_ROOT%\%OT_DLLR%\GlobalSessionService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_LOCAL_SESSION_SERVICE_ROOT%\%OT_DLLR%\SessionService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_GLOBAL_DIRECTORY_SERVICE_ROOT%\%OT_DLLR%\GlobalDirectoryService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_LOCAL_DIRECTORY_SERVICE_ROOT%\%OT_DLLR%\LocalDirectoryService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_UI_SERVICE_ROOT%\%OT_DLLR%\uiFrontend.exe" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_RELAY_SERVICE_ROOT%\%OT_DLLR%\RelayService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_LOGGER_SERVICE_ROOT%\%OT_DLLR%\LoggerService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_AUTHORISATION_SERVICE_ROOT%\%OT_DLLR%\AuthorisationService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_PHREEC_SERVICE_ROOT%\%OT_DLLR%\PHREECService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_KRIGING_SERVICE_ROOT%\%OT_DLLR%\KrigingService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_MODELING_SERVICE_ROOT%\%OT_DLLR%\ModelingService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_VISUALIZATION_SERVICE_ROOT%\%OT_DLLR%\VisualizationService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_FITTD_SERVICE_ROOT%\%OT_DLLR%\FITTDService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT%\%OT_DLLR%\ImportParameterizedDataService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_CARTESIAN_MESH_SERVICE_ROOT%\%OT_DLLR%\CartesianMeshService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_TET_MESH_SERVICE_ROOT%\%OT_DLLR%\TetMeshService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_GETDP_SERVICE_ROOT%\%OT_DLLR%\GetDPService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_PYTHON_EXECUTION_SERVICE_ROOT%\%OT_DLLR%\PythonExecutionService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_PYTHON_EXECUTION_ROOT%\%OT_DLLR%\PythonExecution.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_BLOCKEDITORSERVICE_ROOT%\%OT_DLLR%\BlockEditorService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_DATA_PROCESSING_SERVICE_ROOT%\%OT_DLLR%\DataProcessingService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"


COPY "%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\target\release\open_twin.exe" "%OPENTWIN_DEPLOYMENT_DIR%"

REM ====================================================================
REM  Copy Tools
REM ====================================================================

ECHO Copy Tools
COPY "%OPENTWIN_DEV_ROOT%\Tools\OToolkit\x64\Release\OToolkit.dll" "%OPENTWIN_DEPLOYMENT_DIR%"

REM ====================================================================
REM  Copy Admin Panel
REM ====================================================================

ECHO Copy Admin Panel
XCOPY /E /Q "%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\build\*.*" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"
COPY "%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\Apache_config\.htaccess" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"
COPY /Y "%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\Apache_config\httpd.conf" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\conf"
COPY /Y "%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\Apache_config\httpd-ahssl.conf" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\conf\extra"

GOTO END

:PAUSE_END
pause
GOTO END

:END
