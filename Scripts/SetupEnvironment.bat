REM @ECHO OFF
REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 3. DEVENV_ROOT_2022

REM Ensure that the setup will only be performed once
IF "%OPENTWIN_DEV_ENV_DEFINED%"=="1" (
	goto END
)

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

REM Call third party environment shell
CALL "%OPENTWIN_THIRDPARTY_ROOT%\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_THIRDPARTY_ENV_DEFINED%" == "1" (
	goto END
)

REM #########################################################################################################################################################################################################################################################################################################################################################################################
REM OpenTwin services paths
REM Call the batch file with the following arguments:
REM 1: Name of the library in upper case letters and underscores as delimiter (e.g. "TEST_LIB" will result in "TEST_LIB_ROOT"; "TEST_LIB_INCD"; ...)
REM 2: Name of the library in CamelCase with the same name as the created binaries (<name>.dll) (e.g. "TestLib" will result in debug = "TestLibd.lib" and release = "TestLib.lib")
REM 3: Path to the library root folder without '\' suffix (e.g. "X:\testlib" will be set as root path)
REM See contents of Scripts/SetLibraryEnv.bat for further information
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_AUTHORISATION_SERVICE AuthorisationService %OPENTWIN_DEV_ROOT%\Services\AuthorisationService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_MODEL_SERVICE Model %OPENTWIN_DEV_ROOT%\Services\Model
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_GLOBAL_SESSION_SERVICE GlobalSessionService %OPENTWIN_DEV_ROOT%\Services\GlobalSessionService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_LOCAL_SESSION_SERVICE SessionService %OPENTWIN_DEV_ROOT%\Services\SessionService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_GLOBAL_DIRECTORY_SERVICE GlobalDirectoryService %OPENTWIN_DEV_ROOT%\Services\GlobalDirectoryService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_LOCAL_DIRECTORY_SERVICE LocalDirectoryService %OPENTWIN_DEV_ROOT%\Services\LocalDirectoryService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_RELAY_SERVICE RelayService %OPENTWIN_DEV_ROOT%\Services\RelayService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_LOGGER_SERVICE LoggerService %OPENTWIN_DEV_ROOT%\Services\LoggerService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_PHREEC_SERVICE PHREECService %OPENTWIN_DEV_ROOT%\Services\PHREECService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_KRIGING_SERVICE KrigingService %OPENTWIN_DEV_ROOT%\Services\KrigingService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_MODELING_SERVICE ModelingService %OPENTWIN_DEV_ROOT%\Services\ModelingService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_VISUALIZATION_SERVICE VisualizationService %OPENTWIN_DEV_ROOT%\Services\VisualizationService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_FITTD_SERVICE FITTDService %OPENTWIN_DEV_ROOT%\Services\FITTDService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_CARTESIAN_MESH_SERVICE CartesianMeshService %OPENTWIN_DEV_ROOT%\Services\CartesianMeshService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_TET_MESH_SERVICE TetMeshService %OPENTWIN_DEV_ROOT%\Services\TetMeshService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_IMPORT_PARAMETERIZED_DATA_SERVICE ImportParameterizedData %OPENTWIN_DEV_ROOT%\Services\ImportParameterizedData
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_GETDP_SERVICE GetDPService %OPENTWIN_DEV_ROOT%\Services\GetDPService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_GLOBAL_DIRECTORY_SERVICE GlobalDirectoryService %OPENTWIN_DEV_ROOT%\Services\GlobalDirectoryService
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_UI_SERVICE uiService %OPENTWIN_DEV_ROOT%\Services\uiService

REM #########################################################################################################################################################################################################################################################################################################################################################################################
REM OpenTwin library paths
REM Call the batch file with the following arguments:
REM 1: Name of the library in upper case letters and underscores as delimiter (e.g. "TEST_LIB" will result in "TEST_LIB_ROOT"; "TEST_LIB_INCD"; ...)
REM 2: Name of the library in CamelCase with the same name as the created binaries (<name>.dll) (e.g. "TestLib" will result in debug = "TestLibd.lib" and release = "TestLib.lib")
REM 3: Path to the library root folder without '\' suffix (e.g. "X:\testlib" will be set as root path)
REM See contents of Scripts/SetLibraryEnv.bat for further information

CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_BLOCKEDITOR BlockEditor %OPENTWIN_DEV_ROOT%\Libraries\BlockEditor
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_BLOCKEDITORAPI BlockEditorAPI %OPENTWIN_DEV_ROOT%\Libraries\BlockEditorAPI
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_CADMODELENTITIES CadModelEntities %OPENTWIN_DEV_ROOT%\Libraries\CadModelEntities
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_DATASTORAGE DataStorage %OPENTWIN_DEV_ROOT%\Libraries\DataStorage
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_MODELENTITIES ModelEntities %OPENTWIN_DEV_ROOT%\Libraries\ModelEntities

CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_COMMUNICATION OpenTwinCommunication %OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCommunication
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_CORE OpenTwinCore %OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCore
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_FOUNDATION OpenTwinServiceFoundation %OPENTWIN_DEV_ROOT%\Libraries\OpenTwinServiceFoundation
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_SYSTEM OpenTwinSystem %OPENTWIN_DEV_ROOT%\Libraries\OpenTwinSystem
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_GUI OTGui %OPENTWIN_DEV_ROOT%\Libraries\OTGui
REM CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_WIDGETS OTWidgets %OPENTWIN_DEV_ROOT%\Libraries\OTWidgets

CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_QWTWRAPPER QwtWrapper %OPENTWIN_DEV_ROOT%\Libraries\QwtWrapper
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_RUBBERBANDAPI RubberbandEngineCore %OPENTWIN_DEV_ROOT%\Libraries\RubberbandEngineCore
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_RUBBERBAND RubberbandEngineOsgWrapper %OPENTWIN_DEV_ROOT%\Libraries\RubberbandEngineOsgWrapper
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_UICORE uiCore %OPENTWIN_DEV_ROOT%\Libraries\uiCore
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_UIPLUGINAPI UIPluginAPI %OPENTWIN_DEV_ROOT%\Libraries\UIPluginAPI
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetLibraryEnv.bat" OT_VIEWER Viewer %OPENTWIN_DEV_ROOT%\Libraries\Viewer

REM #########################################################################################################################################################################################################################################################################################################################################################################################
REM OpenTwin tools paths
REM Call the batch file with the following arguments:
REM 1: Name of the library in upper case letters and underscores as delimiter (e.g. "TEST_LIB" will result in "TEST_LIB_ROOT"; "TEST_LIB_INCD"; ...)
REM 2: Name of the library in CamelCase with the same name as the created binaries (<name>.dll) (e.g. "TestLib" will result in debug = "TestLibd.lib" and release = "TestLib.lib")
REM 3: Path to the library root folder without '\' suffix (e.g. "X:\testlib" will be set as root path)
REM See contents of Scripts/SetLibraryEnv.bat for further information
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Other\SetServiceEnv.bat" OT_OTOOLKIT OToolkit %OPENTWIN_DEV_ROOT%\Tools\OToolkit

REM #########################################################################################################################################################################################################################################################################################################################################################################################

REM HANDLING THE SSL CERTS

IF DEFINED OPEN_TWIN_CERTS_PATH (
	SET OPEN_TWIN_CA_CERT=%OPEN_TWIN_CERTS_PATH%\ca.pem
	SET OPEN_TWIN_SERVER_CERT=%OPEN_TWIN_CERTS_PATH%\server.pem
	SET OPEN_TWIN_SERVER_CERT_KEY=%OPEN_TWIN_CERTS_PATH%\server-key.pem
) else (
	SET OPEN_TWIN_CA_CERT=%OPENTWIN_DEV_ROOT%\Certificates\Generated\ca.pem
	SET OPEN_TWIN_SERVER_CERT=%OPENTWIN_DEV_ROOT%\Certificates\Generated\server.pem
	SET OPEN_TWIN_SERVER_CERT_KEY=%OPENTWIN_DEV_ROOT%\Certificates\Generated\server-key.pem
)

REM #########################################################################################################################################################################################################################################################################################################################################################################################
REM OpenTwin default service

REM SET OT_DEFAULT_SERVICE_INCD=%OT_CORE_INCD%;%OT_SYSTEM_INCD%;%OT_COMMUNICATION_INCD%;%OT_FOUNDATION_INCD%;%OT_MODELENTITIES_INCD%;%OT_DATASTORAGE_INCD%;%OT_UIPLUGINAPI_INCD%;%R_JSON_INCD%;%CURL_INCD%;%MONGO_C_ROOT%\Debug\include;%MONGO_CXX_ROOT%\Debug\include;%MONGO_BOOST_ROOT%
REM SET OT_DEFAULT_SERVICE_INCR=%OT_CORE_INCR%;%OT_SYSTEM_INCR%;%OT_COMMUNICATION_INCR%;%OT_FOUNDATION_INCR%;%OT_MODELENTITIES_INCR%;%OT_DATASTORAGE_INCR%;%OT_UIPLUGINAPI_INCR%;%R_JSON_INCR%;%CURL_INCD%;%MONGO_C_ROOT%\Release\include;%MONGO_CXX_ROOT%\Release\include;%MONGO_BOOST_ROOT%
SET OT_DEFAULT_SERVICE_INCD=%OT_CORE_INCD%;%OT_SYSTEM_INCD%;%OT_COMMUNICATION_INCD%;%OT_FOUNDATION_INCD%;%OT_MODELENTITIES_INCD%;%OT_DATASTORAGE_INCD%;%OT_UIPLUGINAPI_INCD%;%R_JSON_INCD%;%CURL_INCD%;%MONGO_C_INCD%;%MONGO_CXX_INCD%;%MONGO_BOOST_INCD%
SET OT_DEFAULT_SERVICE_INCR=%OT_CORE_INCR%;%OT_SYSTEM_INCR%;%OT_COMMUNICATION_INCR%;%OT_FOUNDATION_INCR%;%OT_MODELENTITIES_INCR%;%OT_DATASTORAGE_INCR%;%OT_UIPLUGINAPI_INCR%;%R_JSON_INCR%;%CURL_INCR%;%MONGO_C_INCR%;%MONGO_CXX_INCR%;%MONGO_BOOST_INCR%

REM SET OT_DEFAULT_SERVICE_LIBPATHD=%OT_CORE_ROOT%\x64\Debug;%OT_SYSTEM_ROOT%\x64\Debug;%OT_COMMUNICATION_ROOT%\x64\Debug;%OT_FOUNDATION_ROOT%\x64\Debug;%OT_MODELENTITIES_ROOT%\x64\Debug;%OT_DATASTORAGE_ROOT%\x64\Debug;%OT_UIPLUGINAPI_ROOT%\x64\Debug;%CURL_LIB%;%MONGO_C_ROOT%\Debug\lib;%MONGO_CXX_ROOT%\Debug\lib;%ZLIB_LIB%\Debug\lib
REM SET OT_DEFAULT_SERVICE_LIBPATHR=%OT_CORE_ROOT%\x64\Release;%OT_SYSTEM_ROOT%\x64\Release;%OT_COMMUNICATION_ROOT%\x64\Release;%OT_FOUNDATION_ROOT%\x64\Release;%OT_MODELENTITIES_ROOT%\x64\Release;%OT_DATASTORAGE_ROOT%\x64\Release;%OT_UIPLUGINAPI_ROOT%\x64\Release;%CURL_LIB%;%MONGO_C_ROOT%\Release\lib;%MONGO_CXX_ROOT%\Release\lib;%ZLIB_LIB%\Release\lib
SET OT_DEFAULT_SERVICE_LIBPATHD=%OT_CORE_LIBPATHD%;%OT_SYSTEM_LIBPATHD%;%OT_COMMUNICATION_LIBPATHD%;%OT_FOUNDATION_LIBPATHD%;%OT_MODELENTITIES_LIBPATHD%;%OT_DATASTORAGE_LIBPATHD%;%OT_UIPLUGINAPI_LIBPATHD%;%CURL_LIBPATHD%;%MONGO_C_LIBPATHD%;%MONGO_CXX_LIBPATHD%
SET OT_DEFAULT_SERVICE_LIBPATHR=%OT_CORE_LIBPATHR%;%OT_SYSTEM_LIBPATHR%;%OT_COMMUNICATION_LIBPATHR%;%OT_FOUNDATION_LIBPATHR%;%OT_MODELENTITIES_LIBPATHR%;%OT_DATASTORAGE_LIBPATHR%;%OT_UIPLUGINAPI_LIBPATHR%;%CURL_LIBPATHR%;%MONGO_C_LIBPATHR%;%MONGO_CXX_LIBPATHR%

REM SET OT_DEFAULT_SERVICE_LIBD=OpenTwinCore.lib;OpenTwinSystem.lib;OpenTwinCommunication.lib;OpenTwinServiceFoundation.lib;ModelEntities.lib;DataStorage.lib;libCurl.lib;mongocxx.lib;bsoncxx.lib;mongoc-1.0.lib;bson-1.0.lib
REM SET OT_DEFAULT_SERVICE_LIBR=OpenTwinCore.lib;OpenTwinSystem.lib;OpenTwinCommunication.lib;OpenTwinServiceFoundation.lib;ModelEntities.lib;DataStorage.lib;libCurl.lib;mongocxx.lib;bsoncxx.lib;mongoc-1.0.lib;bson-1.0.lib
SET OT_DEFAULT_SERVICE_LIBD=%OT_CORE_LIBD%;%OT_SYSTEM_LIBD%;%OT_COMMUNICATION_LIBD%;%OT_FOUNDATION_LIBD%;%OT_MODELENTITIES_LIBD%;%OT_DATASTORAGE_LIBD%;%OT_UIPLUGINAPI_LIBD%;%CURL_LIBD%;%MONGO_C_LIBD%;%MONGO_CXX_LIBD%
SET OT_DEFAULT_SERVICE_LIBR=%OT_CORE_LIBR%;%OT_SYSTEM_LIBR%;%OT_COMMUNICATION_LIBR%;%OT_FOUNDATION_LIBR%;%OT_MODELENTITIES_LIBR%;%OT_DATASTORAGE_LIBR%;%OT_UIPLUGINAPI_LIBR%;%CURL_LIBR%;%MONGO_C_LIBR%;%MONGO_CXX_LIBR%

REM SET OT_DEFAULT_SERVICE_DLLD=%OT_CORE_ROOT%\x64\Debug;%OT_SYSTEM_ROOT%\x64\Debug;%OT_COMMUNICATION_ROOT%\x64\Debug;%OT_FOUNDATION_ROOT%\x64\Debug;%OT_MODELENTITIES_ROOT%\x64\Debug;%OT_DATASTORAGE_ROOT%\x64\Debug;%OT_UIPLUGINAPI_ROOT%\x64\Debug;%CURL_DLLD%;%MONGO_C_ROOT%\Debug\bin;%MONGO_CXX_ROOT%\Debug\bin
REM SET OT_DEFAULT_SERVICE_DLLR=%OT_CORE_ROOT%\x64\Release;%OT_SYSTEM_ROOT%\x64\Release;%OT_COMMUNICATION_ROOT%\x64\Release;%OT_FOUNDATION_ROOT%\x64\Release;%OT_MODELENTITIES_ROOT%\x64\Release;%OT_DATASTORAGE_ROOT%\x64\Release;%OT_UIPLUGINAPI_ROOT%\x64\Release;%CURL_DLL%;%MONGO_C_ROOT%\Release\bin;%MONGO_CXX_ROOT%\Release\bin
SET OT_DEFAULT_SERVICE_DLLD=%OT_CORE_DLLD%;%OT_SYSTEM_DLLD%;%OT_COMMUNICATION_DLLD%;%OT_FOUNDATION_DLLD%;%OT_MODELENTITIES_DLLD%;%OT_DATASTORAGE_DLLD%;%OT_UIPLUGINAPI_DLLD%;%CURL_DLLD%;%MONGO_C_DLLD%;%MONGO_CXX_DLLD%
SET OT_DEFAULT_SERVICE_DLLR=%OT_CORE_DLLR%;%OT_SYSTEM_DLLR%;%OT_COMMUNICATION_DLLR%;%OT_FOUNDATION_DLLR%;%OT_MODELENTITIES_DLLR%;%OT_DATASTORAGE_DLLR%;%OT_UIPLUGINAPI_DLLR%;%CURL_DLLR%;%MONGO_C_DLLR%;%MONGO_CXX_DLLR%

REM #########################################################################################################################################################################################################################################################################################################################################################################################
REM OpenTwin all services path

SET OT_ALL_DLLD=%OT_BLOCKEDITOR_DLLD%;%OT_BLOCKEDITORAPI_DLLD%;%OT_CADMODELENTITIES_DLLD%;%OT_DATASTORAGE_DLLD%;%OT_MODELENTITIES_DLLD%;%OT_COMMUNICATION_DLLD%;%OT_CORE_DLLD%;%OT_FOUNDATION_DLLD%;%OT_SYSTEM_DLLD%;%OT_GUI_DLLD%;%OT_WIDGETS_DLLD%;%OT_QWTWRAPPER_DLLD%;%OT_RUBBERBANDAPI_DLLD%;%OT_RUBBERBAND_DLLD%;%OT_UICORE_DLLD%;%OT_UIPLUGINAPI_DLLD%;%OT_VIEWER_DLLD%;

REM #########################################################################################################################################################################################################################################################################################################################################################################################

REM Add Deployment path to the path
SET PATH=%OPENTWIN_DEV_ROOT%\Deployment;%PATH%

REM Set the env defined at the end to ensure everything else was completed successfully
SET OPENTWIN_DEV_ENV_DEFINED=1
ECHO Environment was set up successfully.

GOTO END

REM In case we want the user to see the messages before closing the window
:PAUSE_END
pause
GOTO END

:END
