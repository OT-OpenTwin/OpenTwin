@ECHO OFF

REM The first argument of the shell defines whether a release or debug build shall be performed. "BOTH" (default) , "RELEASE", "DEBUG" 
REM The second argument of hte shell defines whetehr a full rebuild or just a build is performed. "BUILD" (default), "REBUILD"

REM This script requires the following environment variables to be set:
REM 1. SIM_PLAT_ROOT
REM 2. DEVENV_ROOT_2022

IF "%SIM_PLAT_ROOT%"=="" (
	ECHO Please specify the following environment variables: SIM_PLAT_ROOT
	goto END
)

IF "%DEVENV_ROOT_2022%"=="" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto END
)

REM Setup eviroment

CALL "%SIM_PLAT_ROOT%\MasterBuild\set_env.bat"

IF "%OT_DEPLOYMENT_DIR%" == "" (
    SET OT_DEPLOYMENT_DIR="%SIM_PLAT_ROOT%\Deployment"
)

REM Shutdown the session and authorisation services if they are still running 

IF NOT DEFINED OPEN_TWIN_SERVICES_ADDRESS (
	SET OPEN_TWIN_SERVICES_ADDRESS=127.0.0.1
)

IF NOT DEFINED OPEN_TWIN_LOG_PORT (
	SET OPEN_TWIN_GSS_PORT=8090
)

IF NOT DEFINED OPEN_TWIN_GSS_PORT (
	SET OPEN_TWIN_GSS_PORT=8091
)

IF NOT DEFINED OPEN_TWIN_AUTH_PORT (
	SET OPEN_TWIN_AUTH_PORT=8092
)

IF NOT DEFINED OPEN_TWIN_LSS_PORT (
	SET OPEN_TWIN_LSS_PORT=8093
)

REM Shutdown microservices
taskkill /IM "open_twin.exe" /F

REM Shutdown ui
taskkill /IM "uiFrontend.exe" /F

REM Shutdown Apache server
taskkill /IM "httpd.exe" /F

ECHO Delete libraries

DEL "%OT_DEPLOYMENT_DIR%\BlockEditor.dll"
DEL "%OT_DEPLOYMENT_DIR%\BlockEditorAPI.dll"
DEL "%OT_DEPLOYMENT_DIR%\OpenTwinSystem.dll"
DEL "%OT_DEPLOYMENT_DIR%\OpenTwinCore.dll"
DEL "%OT_DEPLOYMENT_DIR%\OpenTwinCommunication.dll"
DEL "%OT_DEPLOYMENT_DIR%\OpenTwinServiceFoundation.dll"
DEL "%OT_DEPLOYMENT_DIR%\ModelEntities.dll"
DEL "%OT_DEPLOYMENT_DIR%\CADModelEntities.dll"
DEL "%OT_DEPLOYMENT_DIR%\Model.dll"
DEL "%OT_DEPLOYMENT_DIR%\Viewer.dll"
DEL "%OT_DEPLOYMENT_DIR%\DataStorage.dll"
DEL "%OT_DEPLOYMENT_DIR%\UIPluginAPI.dll"
DEL "%OT_DEPLOYMENT_DIR%\uiCore.dll"
DEL "%OT_DEPLOYMENT_DIR%\RubberbandEngineCore.dll"	
DEL "%OT_DEPLOYMENT_DIR%\RubberbandOsgWrapper.dll"
DEL "%OT_DEPLOYMENT_DIR%\QwtWrapper.dll"
DEL "%OT_DEPLOYMENT_DIR%\GlobalSessionService.dll"
DEL "%OT_DEPLOYMENT_DIR%\SessionService.dll"
DEL "%OT_DEPLOYMENT_DIR%\GlobalDirectoryService.dll"
DEL "%OT_DEPLOYMENT_DIR%\LocalDirectoryService.dll"
DEL "%OT_DEPLOYMENT_DIR%\uiFrontend.exe"
DEL "%OT_DEPLOYMENT_DIR%\RelayService.dll"
DEL "%OT_DEPLOYMENT_DIR%\LoggerService.dll"
DEL "%OT_DEPLOYMENT_DIR%\AuthorisationService.dll"
DEL "%OT_DEPLOYMENT_DIR%\PHREECService.dll"
DEL "%OT_DEPLOYMENT_DIR%\KrigingService.dll"
DEL "%OT_DEPLOYMENT_DIR%\ModelingService.dll"
DEL "%OT_DEPLOYMENT_DIR%\VisualizationService.dll"
DEL "%OT_DEPLOYMENT_DIR%\FITTDService.dll"
DEL "%OT_DEPLOYMENT_DIR%\ImportParameterizedDataService.dll"
DEL "%OT_DEPLOYMENT_DIR%\CartesianMeshService.dll"
DEL "%OT_DEPLOYMENT_DIR%\TetMeshService.dll"
DEL "%OT_DEPLOYMENT_DIR%\GetDPService.dll"
DEL "%OT_DEPLOYMENT_DIR%\OTGui.dll"
DEL "%OT_DEPLOYMENT_DIR%\open_twin.exe"

DEL "%OT_DEPLOYMENT_DIR%\OToolkit.dll"

ECHO Delete Admin Service

RMDIR /Q /S "%OT_DEPLOYMENT_DIR%\Apache\htdocs"
MKDIR "%OT_DEPLOYMENT_DIR%\Apache\htdocs"

ECHO Copy libraries

COPY "%SIM_PLAT_ROOT%\Libraries\BlockEditor\x64\Release\BlockEditor.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\BlockEditorAPI\x64\Release\BlockEditorAPI.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\OpenTwinCore\x64\Release\OpenTwinCore.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\OpenTwinSystem\x64\Release\OpenTwinSystem.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\OpenTwinCommunication\x64\Release\OpenTwinCommunication.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\OpenTwinServiceFoundation\x64\Release\OpenTwinServiceFoundation.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\ModelEntities\x64\Release\ModelEntities.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\CADModelEntities\x64\Release\CADModelEntities.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\Model\x64\Release\Model.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\Viewer\x64\Release\Viewer.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\DataStorage\x64\Release\DataStorage.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%OT_UIPLUGINAPI_ROOT%\x64\Release\UIPluginAPI.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%UICORE_LIB_ROOT%\x64\Release\uiCore.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%RUBBERBAND_ENGINE_CORE%\x64\Release\RubberbandEngineCore.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%RUBBERBAND_ENGINE_OSG%\x64\Release\RubberbandOsgWrapper.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QWT_WRAPPER%\x64\Release\QwtWrapper.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\GlobalSessionService\x64\Release\GlobalSessionService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\SessionService\x64\Release\SessionService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\GlobalDirectoryService\x64\Release\GlobalDirectoryService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\LocalDirectoryService\x64\Release\LocalDirectoryService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%UISERVICE_LIB_ROOT%\x64\Release\uiFrontend.exe" "%OT_DEPLOYMENT_DIR%"
COPY "%RELAYSERVICE_LIB_ROOT%\x64\Release\RelayService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\LoggerService\x64\Release\LoggerService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\AuthorisationService\x64\Release\AuthorisationService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\PHREECService\x64\Release\PHREECService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\KrigingService\x64\Release\KrigingService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\KrigingService\x64\Release\run.py" "%OT_DEPLOYMENT_DIR%\Python"
COPY "%SIM_PLAT_ROOT%\Libraries\ModelingService\x64\Release\ModelingService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\VisualizationService\x64\Release\VisualizationService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\FITTDService\x64\Release\FITTDService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\ImportParameterizedData\x64\Release\ImportParameterizedDataService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\CartesianMeshService\x64\Release\CartesianMeshService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\TetMeshService\x64\Release\TetMeshService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\GetDPService\x64\Release\GetDPService.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Libraries\OTGui\x64\Release\OTGui.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Microservices\OpenTwin\target\release\open_twin.exe" "%OT_DEPLOYMENT_DIR%"

COPY "%SIM_PLAT_ROOT%\Tools\OToolkit\x64\Release\OToolkit.dll" "%OT_DEPLOYMENT_DIR%"

ECHO Copy Admin Panel

XCOPY /E /Q "%SIM_PLAT_ROOT%\Microservices\AdminPanel\build\*.*" "%OT_DEPLOYMENT_DIR%\Apache\htdocs"
COPY "%SIM_PLAT_ROOT%\Microservices\AdminPanel\Apache_config\.htaccess" "%OT_DEPLOYMENT_DIR%\Apache\htdocs"
COPY /Y "%SIM_PLAT_ROOT%\Microservices\AdminPanel\Apache_config\httpd.conf" "%OT_DEPLOYMENT_DIR%\Apache\conf"
COPY /Y "%SIM_PLAT_ROOT%\Microservices\AdminPanel\Apache_config\httpd-ahssl.conf" "%OT_DEPLOYMENT_DIR%\Apache\conf\extra"

