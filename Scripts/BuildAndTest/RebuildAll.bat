@ECHO OFF

REM The first argument of the shell defines whether a release or debug build shall be performed. BOTH (default) , RELEASE, DEBUG 
REM The second argument of hte shell defines whether a full rebuild or just a build is performed. BUILD, REBUILD (default)
REM Please note that the commands are case-sensitive and that they must not be enclosed in quotes.

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

IF "%QtMsBuild%"=="" (
	ECHO Please install and set up the "Qt Visual Studio Tools" extension in Visual Studio, since it is required for build.
	goto END
)


REM Setup eviroment

CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

cd /D "%OPENTWIN_DEV_ROOT%\Scripts"

REM Create the certificates if necessary 
cd CreateCertificates
CALL createCertificates.bat
cd ..

REM Clean up the build logs 

if "%1"=="" (
	DEL buildLog_Debug.txt
	DEL buildLog_Release.txt
)
	
if "%1"=="BOTH" (
	DEL buildLog_Debug.txt
	DEL buildLog_Release.txt
)
	
if "%1"=="RELEASE" (
	DEL buildLog_Release.txt
)
	
if "%1"=="DEBUG" (
	DEL buildLog_Debug.txt
)

DEL buildLog_Summary.txt
DEL RUSTbuildLog.txt
DEL AdminPanel_buildLog.txt

REM ====================================================================
REM Build the key generator 
REM ====================================================================

ECHO ===============================================================
ECHO Build Key Generator 
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Microservices\KeyGenerator\build.bat" RELEASE REBUILD 

IF NOT EXIST "%OPENTWIN_DEV_ROOT%\Microservices\SSL_certificates\encryptionKey.h" (
	ECHO Updating header file 
	CALL "%OPENTWIN_DEV_ROOT%\Microservices\KeyGenerator\x64\Release\KeyGenerator.exe" 2048 "%OPENTWIN_DEV_ROOT%\Microservices\SSL_certificates\encryptionKey.h"
)

REM ====================================================================
REM Build the libraries 
REM ====================================================================

ECHO ===============================================================
ECHO Build Library: OpenTwinSystem
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinSystem\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: OpenTwinCore
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCore\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: OTGui
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OTGui\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: OpenTwinCommunications
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCommunication\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: BlockEditorAPI
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\BlockEditorAPI\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: BlockEditor
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\BlockEditor\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: DataStorage
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\DataStorage\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: ModelEntities
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\ModelEntities\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: CADModelEntities
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\CADModelEntities\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: ServiceFoundation
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinServiceFoundation\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: uiCore
ECHO ===============================================================
CALL "%UICORE_LIB_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: UI Plugin API
ECHO ===============================================================
CALL "%OT_UIPLUGINAPI_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: RubberbandEngine: Core
ECHO ===============================================================
CALL "%RUBBERBAND_ENGINE_CORE%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: RubberbandEngine: osg Wrapper
ECHO ===============================================================
CALL "%RUBBERBAND_ENGINE_OSG%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: QWT Wrapper
ECHO ===============================================================
CALL "%QWT_WRAPPER%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: Viewer
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\Viewer\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Microservice Launcher: OpenTwin
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Microservices\OpenTwin\build.bat" %1 %2 > RUSTbuildLog.txt

REM ====================================================================
REM Build the services
REM ====================================================================

ECHO ===============================================================
ECHO Build Service: Model
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\Model\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: GlobalSessionService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\GlobalSessionService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: LocalSessionService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\SessionService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: GlobalDirectoryService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\GlobalDirectoryService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: LocalDirectoryService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\LocalDirectoryService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: RelayService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\RelayService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: LoggerService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\LoggerService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: AuthorisationService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\AuthorisationService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: PHREECService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\PHREECService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: KrigingService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\KrigingService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: ModelingService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\ModelingService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: VisualizationService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\VisualizationService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: FITTDService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\FITTDService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: CartesianMeshService
ECHO ===============================================================
CALL  "%OPENTWIN_DEV_ROOT%\Libraries\CartesianMeshService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: TetMeshService
ECHO ===============================================================
CALL  "%OPENTWIN_DEV_ROOT%\Libraries\TetMeshService\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: ImportParameterizedData
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\ImportParameterizedData\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: GetDPService
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Libraries\GetDPService\build.bat" %1 %2

REM ====================================================================
REM Build the executables
REM ====================================================================

ECHO ===============================================================
ECHO Build Executable: uiFrontend
ECHO ===============================================================
CALL "%UISERVICE_LIB_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Executable: OToolkit
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\OToolkit\build.bat" %1 %2

ECHO ====================================================================
ECHO Build Admin Panel
ECHO ====================================================================
PUSHD "%OPENTWIN_DEV_ROOT%\Microservices\AdminPanel"
CALL "%OPENTWIN_DEV_ROOT%\Microservices\AdminPanel\build.bat" > "%OPENTWIN_DEV_ROOT%\MasterBuild\AdminPanel_buildLog.txt"
POPD

REM ECHO ===============================================================
REM ECHO Build Playground: UI_test - Executable
REM ECHO ===============================================================
REM CALL "%OPENTWIN_DEV_ROOT%\Playground\UI_test\build_exe.bat" %1 %2

REM ECHO ===============================================================
REM ECHO Build Playground: UI_test - DLL
REM ECHO ===============================================================
REM CALL "%OPENTWIN_DEV_ROOT%\Playground\UI_test\build_dll.bat" %1 %2

REM ====================================================================
REM Create the buildlog summary
REM ====================================================================

if "%2"=="" (
	SET searchString="= Rebuild All:"
) ELSE IF "%2"=="REBUILD" (
	SET searchString="= Rebuild All:"
) ELSE (
	SET searchString="= Build:"
)

FIND %searchString% buildLog_Debug.txt buildLog_Release.txt > buildLog_Summary.txt
FIND """message"":" RUSTbuildLog.txt >> buildLog_Summary.txt

EXIT /b 0
