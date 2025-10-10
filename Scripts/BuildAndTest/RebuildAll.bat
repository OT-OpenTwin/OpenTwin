@ECHO OFF

REM The first argument of the shell defines whether a release or debug build shall be performed. BOTH (default) , RELEASE, DEBUG 
REM The second argument of hte shell defines whether a full rebuild or just a build is performed. BUILD, REBUILD (default)
REM Please note that the commands are case-sensitive and that they must not be enclosed in quotes.

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. DEVENV_ROOT_2022

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto PAUSE_END
)

IF "%DEVENV_ROOT_2022%"=="" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto END
)

IF "%QtMsBuild%"=="" (
	ECHO Please install and set up the "Qt Visual Studio Tools" extension in Visual Studio, since it is required for build.
	goto END
)

REM ====================================================================
REM Preparations for the build process 
REM ====================================================================

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)

if "%1"=="--doc-only" (
	goto DOC_BUILD_ONLY
)

REM Get the current timestamp (capture echo call from the batch)
for /f "delims=" %%a in ('call "%OPENTWIN_DEV_ROOT%\Scripts\Other\PrintCurrentTimestamp.bat" "Build started at: " ""') do set OT_LCL_BuildStart=%%a
echo %OT_LCL_BuildStart%

REM Create the certificates if necessary 
cd /D "%OPENTWIN_DEV_ROOT%\Certificates\CreateLocalCertificates"
CALL CreateLocalCertificates.bat

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"

REM Clean up the build logs 

if "%1"=="" (
	DEL buildLog_Debug.txt 2> nul
	DEL buildLog_Release.txt 2> nul
)
	
if "%1"=="BOTH" (
	DEL buildLog_Debug.txt 2> nul
	DEL buildLog_Release.txt 2> nul
)
	
if "%1"=="RELEASE" (
	DEL buildLog_Release.txt 2> nul
)
	
if "%1"=="DEBUG" (
	DEL buildLog_Debug.txt 2> nul
)

DEL buildLog_Summary.txt 2> nul
DEL RUSTbuildLog.txt 2> nul
DEL AdminPanel_buildLog.txt 2> nul
DEL Documentation_buildLog.txt 2> nul
DEL DoxygenDocumentation_buildLog.txt 2> nul

REM ====================================================================
REM Build the libraries 
REM ====================================================================

ECHO ===============================================================
ECHO Build Library: OpenTwinSystem
ECHO ===============================================================
CALL "%OT_SYSTEM_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: OpenTwinCore
ECHO ===============================================================
CALL "%OT_CORE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: OTRandom
ECHO ===============================================================
CALL "%OT_RANDOM_ROOT%\build.bat" %1 %2

REM ====================================================================
REM Build the key generator and the encryption key, if needed
REM ====================================================================

ECHO ===============================================================
ECHO Build Key Generator 
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\build.bat" RELEASE REBUILD

IF NOT EXIST "%OT_ENCRYPTIONKEY_ROOT%\OTEncryptionKey.h" (
	ECHO Updating header file "%OT_ENCRYPTIONKEY_ROOT%\OTEncryptionKey.h"
	
	REM Remove libraries
	DEL "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release\OTSystem.dll"
	DEL "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release\OTCore.dll"
	DEL "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release\OTRandom.dll"
	
	REM Copy libraries
	COPY "%OT_SYSTEM_ROOT%\%OT_DLLR%\OTSystem.dll" "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release"
	COPY "%OT_CORE_ROOT%\%OT_DLLR%\OTCore.dll" "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release"
	COPY "%OT_RANDOM_ROOT%\%OT_DLLR%\OTRandom.dll" "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release"

	CALL "%OPENTWIN_DEV_ROOT%\Tools\KeyGenerator\x64\Release\KeyGenerator.exe" 2048 "%OT_ENCRYPTIONKEY_ROOT%\OTEncryptionKey.h"
)

REM ====================================================================
REM Build the libraries 
REM ====================================================================

ECHO ===============================================================
ECHO Build Library: OTGui
ECHO ===============================================================
CALL "%OT_GUI_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: OpenTwinCommunications
ECHO ===============================================================
CALL "%OT_COMMUNICATION_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: OTGuiAPI
ECHO ===============================================================
CALL "%OT_GUIAPI_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: OTWidgets
ECHO ===============================================================
CALL "%OT_WIDGETS_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: DataStorage
ECHO ===============================================================
CALL "%OT_DATASTORAGE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: ModelEntities
ECHO ===============================================================
CALL "%OT_MODELENTITIES_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: CADModelEntities
ECHO ===============================================================
CALL "%OT_CADMODELENTITIES_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: BlockEntities
ECHO ===============================================================
CALL "%OT_BLOCKENTITIES_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: OTModelAPI
ECHO ===============================================================
CALL "%OT_MODELAPI_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: ServiceFoundation
ECHO ===============================================================
CALL "%OT_FOUNDATION_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: uiCore
ECHO ===============================================================
CALL "%OT_UICORE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: RubberbandEngine: Core
ECHO ===============================================================
CALL "%OT_RUBBERBANDAPI_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: RubberbandEngine: osg Wrapper
ECHO ===============================================================
CALL "%OT_RUBBERBAND_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: Viewer
ECHO ===============================================================
CALL "%OT_VIEWER_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: Result Data Access
ECHO ===============================================================
CALL "%OT_RESULT_DATA_ACCESS_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: Frontend Connector API
ECHO ===============================================================
CALL "%OT_FRONTEND_CONNECTOR_API_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: StudioSuiteConnector
ECHO ===============================================================
CALL "%OT_STUDIO_SUITE_CONNECTOR_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: LTSpiceConnector
ECHO ===============================================================
CALL "%OT_LTSPICE_CONNECTOR_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Microservice Launcher: OpenTwin
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\build.bat" %1 %2 > RUSTbuildLog.txt

REM ====================================================================
REM Build the services
REM ====================================================================

ECHO ===============================================================
ECHO Build Service: Model
ECHO ===============================================================
CALL "%OT_MODEL_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: GlobalSessionService
ECHO ===============================================================
CALL "%OT_GLOBAL_SESSION_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: LocalSessionService
ECHO ===============================================================
CALL "%OT_LOCAL_SESSION_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: GlobalDirectoryService
ECHO ===============================================================
CALL "%OT_GLOBAL_DIRECTORY_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: LocalDirectoryService
ECHO ===============================================================
CALL "%OT_LOCAL_DIRECTORY_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: RelayService
ECHO ===============================================================
CALL "%OT_RELAY_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: LoggerService
ECHO ===============================================================
CALL "%OT_LOGGER_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: AuthorisationService
ECHO ===============================================================
CALL "%OT_AUTHORISATION_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: PHREECService
ECHO ===============================================================
CALL "%OT_PHREEC_SERVICE_ROOT%\build.bat" %1 %2

REM ECHO ===============================================================
REM ECHO Build Service: KrigingService
REM ECHO ===============================================================
REM CALL "%OT_KRIGING_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: ModelingService
ECHO ===============================================================
CALL "%OT_MODELING_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: VisualizationService
ECHO ===============================================================
CALL "%OT_VISUALIZATION_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: FITTDService
ECHO ===============================================================
CALL "%OT_FITTD_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: CartesianMeshService
ECHO ===============================================================
CALL  "%OT_CARTESIAN_MESH_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: TetMeshService
ECHO ===============================================================
CALL  "%OT_TET_MESH_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: ImportParameterizedData
ECHO ===============================================================
CALL "%OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: GetDPService
ECHO ===============================================================
CALL "%OT_GETDP_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: ElmerFEMService
ECHO ===============================================================
CALL "%OT_ELMERFEM_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: StudioSuiteService
ECHO ===============================================================
CALL "%OT_STUDIOSUITE_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: LTSpiceService
ECHO ===============================================================
CALL "%OT_LTSPICE_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: PyritService
ECHO ===============================================================
CALL "%OT_PYRIT_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: PythonExecutionService
ECHO ===============================================================
CALL "%OT_PYTHON_EXECUTION_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: PythonExecution
ECHO ===============================================================
CALL "%OT_PYTHON_EXECUTION_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: DebugService
ECHO ===============================================================
CALL "%OT_DEBUGSERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: DataProcessingService
ECHO ===============================================================
CALL "%OT_DATA_PROCESSING_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: CircuitSimulatorService
ECHO ===============================================================
CALL "%OT_CIRCUIT_SIMULATOR_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: CircuitExecution
ECHO ===============================================================
CALL "%OT_CIRCUIT_EXECUTION_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Service: LibraryManagementService
ECHO ===============================================================
CALL "%OT_LIBRARY_MANAGEMENT_SERVICE_ROOT%\build.bat" %1 %2

REM ====================================================================
REM Build the executables
REM ====================================================================

ECHO ===============================================================
ECHO Build Executable: uiFrontend
ECHO ===============================================================
CALL "%OT_UI_SERVICE_ROOT%\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Library: OToolkitAPI
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\OToolkitAPI\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Executable: OToolkit
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\OToolkit\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Executable: System Information Tool
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\OTSystemInformationTool\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Executable: Password encryption tool
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\PasswordEncryption\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Executable: SetPermissions (Installation Helper)
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\SetPermissions\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Executable: ConfigMongoDBNoAuth (Installation Helper)
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\ConfigMongoDBNoAuth\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Executable: ConfigMongoDBWithAuth (Installation Helper)
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\ConfigMongoDBWithAuth\build.bat" %1 %2

ECHO ===============================================================
ECHO Build Executable: ModelLibraryUpdater
ECHO ===============================================================
CALL "%OPENTWIN_DEV_ROOT%\Tools\ModelLibraryUpdater\build.bat" %1 %2

ECHO ====================================================================
ECHO Build Admin Panel
ECHO ====================================================================
PUSHD "%OPENTWIN_DEV_ROOT%\Tools\AdminPanel"
CALL "%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\build.bat" > "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\AdminPanel_buildLog.txt"
POPD

REM ====================================================================
REM Create the buildlog summary
REM ====================================================================

ECHO %OT_LCL_BuildStart% >> buildLog_Summary.txt

REM Get the current timestamp (capture echo call from the batch)
for /f "delims=" %%a in ('call "%OPENTWIN_DEV_ROOT%\Scripts\Other\PrintCurrentTimestamp.bat" "Build finished at: " ""') do set OT_LCL_BuildEnd=%%a
ECHO %OT_LCL_BuildEnd%
ECHO %OT_LCL_BuildEnd% >> buildLog_Summary.txt

ECHO. >> buildLog_Summary.txt
ECHO ==================================================================== >> buildLog_Summary.txt
ECHO Release Builds >> buildLog_Summary.txt
ECHO ==================================================================== >> buildLog_Summary.txt

FIND "--- Build " buildLog_Release.txt >> buildLog_Summary.txt

ECHO. >> buildLog_Summary.txt
ECHO ==================================================================== >> buildLog_Summary.txt
ECHO Debug Builds >> buildLog_Summary.txt
ECHO ==================================================================== >> buildLog_Summary.txt

FIND "--- Build " buildLog_Debug.txt >> buildLog_Summary.txt

ECHO. >> buildLog_Summary.txt
ECHO ==================================================================== >> buildLog_Summary.txt
ECHO Rust Builds >> buildLog_Summary.txt
ECHO ==================================================================== >> buildLog_Summary.txt

FIND """message"":" RUSTbuildLog.txt >> buildLog_Summary.txt

EXIT /b 0

GOTO END

:PAUSE_END
pause
GOTO END

:END


IF "%2"=="BUILD"  (
	IF "%OT_SKIP_DOC_ON_BUILD%"=="true" (
		GOTO SKIP_DOC_BUILD
	)
)

:DOC_BUILD_ONLY

CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\BuildDocumentation.bat"

:SKIP_DOC_BUILD