@ECHO OFF

REM The first argument of the shell defines whether a release or debug build shall be performed. "BOTH" (default) , "RELEASE", "DEBUG" 
REM The second argument of hte shell defines whetehr a full rebuild or just a build is performed. "BUILD" (default), "REBUILD"

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. DEVENV_ROOT_2022

IF "%OPENTWIN_DEV_ROOT%"=="" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto END
)

IF "%DEVENV_ROOT_2022%"=="" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto END
)

REM Setup eviroment

CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

IF "%OPENTWIN_DEPLOYMENT_DIR%" == "" (
    SET OPENTWIN_DEPLOYMENT_DIR="%OPENTWIN_DEV_ROOT%\Deployment"
)

CALL "%OPENTWIN_DEV_ROOT%\Scripts\ShutodwnAll.bat"

ECHO Delete libraries

DEL "%OPENTWIN_DEPLOYMENT_DIR%\BlockEditor.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\BlockEditorAPI.dll"
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
DEL "%OPENTWIN_DEPLOYMENT_DIR%\RubberbandOsgWrapper.dll"
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
DEL "%OPENTWIN_DEPLOYMENT_DIR%\ImportParameterizedDataService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\CartesianMeshService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\TetMeshService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\GetDPService.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\OTGui.dll"
DEL "%OPENTWIN_DEPLOYMENT_DIR%\open_twin.exe"

DEL "%OPENTWIN_DEPLOYMENT_DIR%\OToolkit.dll"

ECHO Delete Admin Service

RMDIR /Q /S "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"
MKDIR "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"

REM  Copy libraries
ECHO Copy libraries

COPY "%OT_BLOCKEDITOR_DLLR%\BlockEditor.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_BLOCKEDITORAPI_DLLR%\BlockEditorAPI.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_CORE_DLLR%\OpenTwinCore.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_SYSTEM_DLLR%\OpenTwinSystem.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_COMMUNICATION_DLLR%\OpenTwinCommunication.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_FOUNDATION_DLLR%OpenTwinServiceFoundation.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_MODELENTITIES_DLLR%\ModelEntities.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_CADMODELENTITIES_DLLR%\CADModelEntities.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_VIEWER_DLLR%\Viewer.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_DATASTORAGE_DLLR%\DataStorage.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_UIPLUGINAPI_DLLR%\UIPluginAPI.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_UICORE_DLLR%\uiCore.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_RUBBERBANDAPI_DLLR%\RubberbandEngineCore.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_RUBBERBAND_DLLR%\RubberbandOsgWrapper.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_QWTWRAPPER_DLLR%\QwtWrapper.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_GUI_DLLR%\OTGui.dll" "%OPENTWIN_DEPLOYMENT_DIR%"

REM  Copy Services
ECHO Copy Services

COPY "%OPENTWIN_DEV_ROOT%\Services\Model\x64\Release\Model.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\GlobalSessionService\x64\Release\GlobalSessionService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\SessionService\x64\Release\SessionService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\GlobalDirectoryService\x64\Release\GlobalDirectoryService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\LocalDirectoryService\x64\Release\LocalDirectoryService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%UISERVICE_LIB_ROOT%\x64\Release\uiFrontend.exe" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%RELAYSERVICE_LIB_ROOT%\x64\Release\RelayService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\LoggerService\x64\Release\LoggerService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\AuthorisationService\x64\Release\AuthorisationService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\PHREECService\x64\Release\PHREECService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\KrigingService\x64\Release\KrigingService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\KrigingService\x64\Release\run.py" "%OPENTWIN_DEPLOYMENT_DIR%\Python"
COPY "%OPENTWIN_DEV_ROOT%\Services\ModelingService\x64\Release\ModelingService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\VisualizationService\x64\Release\VisualizationService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\FITTDService\x64\Release\FITTDService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\ImportParameterizedData\x64\Release\ImportParameterizedDataService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\CartesianMeshService\x64\Release\CartesianMeshService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\TetMeshService\x64\Release\TetMeshService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OPENTWIN_DEV_ROOT%\Services\GetDPService\x64\Release\GetDPService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"

COPY "%OPENTWIN_DEV_ROOT%\Microservices\OpenTwin\target\release\open_twin.exe" "%OPENTWIN_DEPLOYMENT_DIR%"

REM  Copy Tools
ECHO Copy Tools
COPY "%OPENTWIN_DEV_ROOT%\Tools\OToolkit\x64\Release\OToolkit.dll" "%OPENTWIN_DEPLOYMENT_DIR%"

REM  Copy Admin Panel
ECHO Copy Admin Panel
XCOPY /E /Q "%OPENTWIN_DEV_ROOT%\Microservices\AdminPanel\build\*.*" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"
COPY "%OPENTWIN_DEV_ROOT%\Microservices\AdminPanel\Apache_config\.htaccess" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs"
COPY /Y "%OPENTWIN_DEV_ROOT%\Microservices\AdminPanel\Apache_config\httpd.conf" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\conf"
COPY /Y "%OPENTWIN_DEV_ROOT%\Microservices\AdminPanel\Apache_config\httpd-ahssl.conf" "%OPENTWIN_DEPLOYMENT_DIR%\Apache\conf\extra"

