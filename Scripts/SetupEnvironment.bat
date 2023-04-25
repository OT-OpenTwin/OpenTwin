REM @ECHO OFF
REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 3. DEVENV_ROOT_2022

REM Ensure that the setup will only be performed once
if "%OPENTWIN_DEV_ENV_DEFINED%"=="1" (
	goto END
)

SET OPENTWIN_DEV_ENV_DEFINED=1

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto END
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto END
)

IF "%DEVENV_ROOT_2022%" == "" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto END
)

REM Call third party environment shell
CALL "%OPENTWIN_THIRDPARTY_ROOT%\SetupEnvironment.bat"

REM ##########################################################################################################
REM OpenTwin project paths
REM Call the batch file with the following arguments:
REM 1: Name of the library in upper case letters and underscores as delimiter (e.g. "TEST_LIB" will result in "TEST_LIB_ROOT"; "TEST_LIB_INCD"; ...)
REM 2: Name of the library in CamelCase with the same name as the created binaries (<name>.dll) (e.g. "TestLib" will result in debug = "TestLibd.lib" and release = "TestLib.lib")
REM 3: Path to the library root folder without '\' suffix (e.g. "X:\testlib" will be set as root path)
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_CORE OpenTwinCore %OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCore
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_SYSTEM OpenTwinSystem %OPENTWIN_DEV_ROOT%\Libraries\OpenTwinSystem
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_GUI OTGui %OPENTWIN_DEV_ROOT%\Libraries\OTGui
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_WIDGETS OTWidgets %OPENTWIN_DEV_ROOT%\Libraries\OTWidgets
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_COMMUNICATION OpenTwinCommunication %OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCommunication
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_FOUNDATION OpenTwinServiceFoundation %OPENTWIN_DEV_ROOT%\Libraries\OpenTwinServiceFoundation
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_MODELENTITIES ModelEntities %OPENTWIN_DEV_ROOT%\Libraries\ModelEntities
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_CADMODELENTITIES CadModelEntities %OPENTWIN_DEV_ROOT%\Libraries\CadModelEntities
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_DATASTORAGE DataStorage %OPENTWIN_DEV_ROOT%\Libraries\DataStorage
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_UIPLUGINAPI UIPluginAPI %OPENTWIN_DEV_ROOT%\Libraries\UIPluginAPI
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_UIPLUGINTEMPLATE UIPluginTemplate %OPENTWIN_DEV_ROOT%\Libraries\UIPluginTemplate
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_BLOCKEDITOR BlockEditor %OPENTWIN_DEV_ROOT%\Libraries\BlockEditor
CALL "%OPEN_TWIN_DEV_ROOT%\MasterBuild\SetLibraryEnv.bat" OT_BLOCKEDITORAPI BlockEditorAPI %OPENTWIN_DEV_ROOT%\Libraries\BlockEditorAPI

REM ##########################################################################################################

REM uiCore and uiWrapper Root Directory
SET UICORE_LIB_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\uiCore
SET UISERVICE_LIB_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\uiService

REM RelayService Root Directory
SET RELAYSERVICE_LIB_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\RelayService

REM Set rubberband directory
SET RUBBERBAND_ENGINE_CORE=%OPENTWIN_DEV_ROOT%\Libraries\RubberbandEngineCore
SET RUBBERBAND_ENGINE_OSG=%OPENTWIN_DEV_ROOT%\Libraries\RubberbandEngineOsgWrapper

REM ##########################################################################################################

REM Set UI_CORE Project settings

REM SET UICORE_PROJ_ENV_INC=%QDIR%\include\QtCore\; %QDIR%\include\QtGui\; %QDIR%\include\QtWidgets\; %QT_TT_ROOT%\include\; %R_JSON_ROOT%\include\; %CURL_INC%
SET UICORE_PROJ_ENV_INC=%QDIR%\include\QtCore\; %QDIR%\include\QtGui\; %QDIR%\include\QtWidgets\; %QT_TT_ROOT%\include\; %R_JSON_ROOT%\include\;

SET UICORE_ENV_INC=%UICORE_LIB_ROOT%\include; $(UICORE_PROJ_ENV_INC)

REM SET UICORE_PROJ_ENV_LIB_ROOT=%QT_TT_ROOT%\src\TabToolbar\Release; %QDIR%\lib; %CURL_LIB%; %ZLIB_LIB%\Release\lib
SET UICORE_PROJ_ENV_LIB_ROOT=%QT_TT_ROOT%\src\TabToolbar\Release; %QDIR%\lib;

REM SET UICORE_PROJ_ENV_LIBD_ROOT=%QT_TT_ROOT%\src\TabToolbar\Debug; %QDIR%\lib; %CURL_LIB%; %ZLIB_LIB%\Debug\lib
SET UICORE_PROJ_ENV_LIBD_ROOT=%QT_TT_ROOT%\src\TabToolbar\Debug; %QDIR%\lib;

SET UICORE_ENV_LIB_ROOT=%UICORE_LIB_ROOT%\x64\Release;$(UICORE_PROJ_ENV_LIB_ROOT)
SET UICORE_ENV_LIBD_ROOT=%UICORE_LIB_ROOT%\x64\Debug;$(UICORE_PROJ_ENV_LIBD_ROOT)

REM SET UICORE_PROJ_ENV_LIB_INP=%QT_TT_ROOT%\src\TabToolbar\Release\TabToolbar.lib; %CURL_LIB%\libCurl.lib; zlib.lib
SET UICORE_PROJ_ENV_LIB_INP=%QT_TT_ROOT%\src\TabToolbar\Release\TabToolbar.lib;

REM SET UICORE_PROJ_ENV_LIBD_INP=%QT_TT_ROOT%\src\TabToolbar\Debug\TabToolbard.lib; %CURL_LIB%\libCurl.lib; zlibd.lib
SET UICORE_PROJ_ENV_LIBD_INP=%QT_TT_ROOT%\src\TabToolbar\Debug\TabToolbard.lib;

SET UICORE_ENV_LIB_INP=%UICORE_LIB_ROOT%\x64\Release\uiCore.lib; $(UICORE_PROJ_ENV_LIB_INP)
SET UICORE_ENV_LIBD_INP=%UICORE_LIB_ROOT%\x64\Debug\uiCore.lib; $(UICORE_PROJ_ENV_LIBD_INP)

REM SET UICORE_ENV_DLL=%UICORE_LIB_ROOT%\x64\Release;%QT_TT_ROOT%\DLL;%CURL_DLL%; %ZLIB_LIB%
SET UICORE_ENV_DLL=%UICORE_LIB_ROOT%\x64\Release;%QT_TT_ROOT%\DLL;

REM SET UICORE_ENV_DLLD=%UICORE_LIB_ROOT%\x64\Debug;%QT_TT_ROOT%\src\TabToolbar\Debug\;
SET UICORE_ENV_DLLD=%UICORE_LIB_ROOT%\x64\Debug;%QT_TT_ROOT%\src\TabToolbar\Debug\;%CURL_DLL%;%ZLIB_LIB%

SET PATH=%OPENTWIN_DEV_ROOT%\Deployment;%PATH%

REM HANDLING THE SSL CERTS

IF DEFINED OPEN_TWIN_CERTS_PATH (
	SET OPEN_TWIN_CA_CERT=%OPEN_TWIN_CERTS_PATH%\ca.pem
	SET OPEN_TWIN_SERVER_CERT=%OPEN_TWIN_CERTS_PATH%\server.pem
	SET OPEN_TWIN_SERVER_CERT_KEY=%OPEN_TWIN_CERTS_PATH%\server-key.pem
) else (
	SET OPEN_TWIN_CA_CERT=%OPENTWIN_DEV_ROOT%\Microservices\SSL_certificates\ca.pem
	SET OPEN_TWIN_SERVER_CERT=%OPENTWIN_DEV_ROOT%\Microservices\SSL_certificates\server.pem
	SET OPEN_TWIN_SERVER_CERT_KEY=%OPENTWIN_DEV_ROOT%\Microservices\SSL_certificates\server-key.pem
)

REM ##########################################################################################################
REM OpenTwin default service

SET OT_DEFAULT_SERVICE_INCD=%OT_CORE_ROOT%\include;%OT_SYSTEM_ROOT%\include;%OT_COMMUNICATION_ROOT%\include;%OT_FOUNDATION_ROOT%\include;%OT_MODELENTITIES_ROOT%\include;%OT_DATASTORAGE_ROOT%\include;%OT_UIPLUGINAPI_ROOT%\include;%R_JSON_ROOT%\include;%CURL_INC%;%MONGO_C_ROOT%\Debug\include;%MONGO_CXX_ROOT%\Debug\include;%MONGO_BOOST_ROOT%
SET OT_DEFAULT_SERVICE_INCR=%OT_CORE_ROOT%\include;%OT_SYSTEM_ROOT%\include;%OT_COMMUNICATION_ROOT%\include;%OT_FOUNDATION_ROOT%\include;%OT_MODELENTITIES_ROOT%\include;%OT_DATASTORAGE_ROOT%\include;%OT_UIPLUGINAPI_ROOT%\include;%R_JSON_ROOT%\include;%CURL_INC%;%MONGO_C_ROOT%\Release\include;%MONGO_CXX_ROOT%\Release\include;%MONGO_BOOST_ROOT%

REM SET OT_DEFAULT_SERVICE_LIBPATHD=%OT_CORE_ROOT%\x64\Debug;%OT_SYSTEM_ROOT%\x64\Debug;%OT_COMMUNICATION_ROOT%\x64\Debug;%OT_FOUNDATION_ROOT%\x64\Debug;%OT_MODELENTITIES_ROOT%\x64\Debug;%OT_DATASTORAGE_ROOT%\x64\Debug;%OT_UIPLUGINAPI_ROOT%\x64\Debug;%CURL_LIB%;%MONGO_C_ROOT%\Debug\lib;%MONGO_CXX_ROOT%\Debug\lib;%ZLIB_LIB%\Debug\lib
REM SET OT_DEFAULT_SERVICE_LIBPATHR=%OT_CORE_ROOT%\x64\Release;%OT_SYSTEM_ROOT%\x64\Release;%OT_COMMUNICATION_ROOT%\x64\Release;%OT_FOUNDATION_ROOT%\x64\Release;%OT_MODELENTITIES_ROOT%\x64\Release;%OT_DATASTORAGE_ROOT%\x64\Release;%OT_UIPLUGINAPI_ROOT%\x64\Release;%CURL_LIB%;%MONGO_C_ROOT%\Release\lib;%MONGO_CXX_ROOT%\Release\lib;%ZLIB_LIB%\Release\lib

SET OT_DEFAULT_SERVICE_LIBPATHD=%OT_CORE_ROOT%\x64\Debug;%OT_SYSTEM_ROOT%\x64\Debug;%OT_COMMUNICATION_ROOT%\x64\Debug;%OT_FOUNDATION_ROOT%\x64\Debug;%OT_MODELENTITIES_ROOT%\x64\Debug;%OT_DATASTORAGE_ROOT%\x64\Debug;%OT_UIPLUGINAPI_ROOT%\x64\Debug;%CURL_LIB%;%MONGO_C_ROOT%\Debug\lib;%MONGO_CXX_ROOT%\Debug\lib
SET OT_DEFAULT_SERVICE_LIBPATHR=%OT_CORE_ROOT%\x64\Release;%OT_SYSTEM_ROOT%\x64\Release;%OT_COMMUNICATION_ROOT%\x64\Release;%OT_FOUNDATION_ROOT%\x64\Release;%OT_MODELENTITIES_ROOT%\x64\Release;%OT_DATASTORAGE_ROOT%\x64\Release;%OT_UIPLUGINAPI_ROOT%\x64\Release;%CURL_LIB%;%MONGO_C_ROOT%\Release\lib;%MONGO_CXX_ROOT%\Release\lib

REM SET OT_DEFAULT_SERVICE_LIBD=OpenTwinCore.lib;OpenTwinSystem.lib;OpenTwinCommunication.lib;OpenTwinServiceFoundation.lib;BlockEditorAPI.lib;ModelEntities.lib;DataStorage.lib;libCurl.lib;mongocxx.lib;bsoncxx.lib;mongoc-1.0.lib;bson-1.0.lib;zlibd.lib
REM SET OT_DEFAULT_SERVICE_LIBR=OpenTwinCore.lib;OpenTwinSystem.lib;OpenTwinCommunication.lib;OpenTwinServiceFoundation.lib;BlockEditorAPI.lib;ModelEntities.lib;DataStorage.lib;libCurl.lib;mongocxx.lib;bsoncxx.lib;mongoc-1.0.lib;bson-1.0.lib;zlib.lib

SET OT_DEFAULT_SERVICE_LIBD=OpenTwinCore.lib;OpenTwinSystem.lib;OpenTwinCommunication.lib;OpenTwinServiceFoundation.lib;ModelEntities.lib;DataStorage.lib;libCurl.lib;mongocxx.lib;bsoncxx.lib;mongoc-1.0.lib;bson-1.0.lib
SET OT_DEFAULT_SERVICE_LIBR=OpenTwinCore.lib;OpenTwinSystem.lib;OpenTwinCommunication.lib;OpenTwinServiceFoundation.lib;ModelEntities.lib;DataStorage.lib;libCurl.lib;mongocxx.lib;bsoncxx.lib;mongoc-1.0.lib;bson-1.0.lib

SET OT_DEFAULT_SERVICE_DLLD=%OT_CORE_ROOT%\x64\Debug;%OT_SYSTEM_ROOT%\x64\Debug;%OT_COMMUNICATION_ROOT%\x64\Debug;%OT_FOUNDATION_ROOT%\x64\Debug;%OT_MODELENTITIES_ROOT%\x64\Debug;%OT_DATASTORAGE_ROOT%\x64\Debug;%OT_UIPLUGINAPI_ROOT%\x64\Debug;%CURL_DLLD%;%MONGO_C_ROOT%\Debug\bin;%MONGO_CXX_ROOT%\Debug\bin
SET OT_DEFAULT_SERVICE_DLLR=%OT_CORE_ROOT%\x64\Release;%OT_SYSTEM_ROOT%\x64\Release;%OT_COMMUNICATION_ROOT%\x64\Release;%OT_FOUNDATION_ROOT%\x64\Release;%OT_MODELENTITIES_ROOT%\x64\Release;%OT_DATASTORAGE_ROOT%\x64\Release;%OT_UIPLUGINAPI_ROOT%\x64\Release;%CURL_DLL%;%MONGO_C_ROOT%\Release\bin;%MONGO_CXX_ROOT%\Release\bin

REM ##########################################################################################################

ECHO Environment was set up successfully.

:END

