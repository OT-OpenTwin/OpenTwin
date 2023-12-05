REM @ECHO OFF
REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT

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

REM Call third party environment shell
CALL "%OPENTWIN_THIRDPARTY_ROOT%\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_THIRDPARTY_ENV_DEFINED%" == "1" (
	goto END
)

SET OT_ENCRYPTIONKEY_ROOT=%OPENTWIN_DEV_ROOT%\Certificates\Generated

REM Set relative path variables

SET OT_INC=include
SET OT_SRC=src
SET OT_DLLD=x64\Debug
SET OT_DLLR=x64\Release
SET OT_LIBD=x64\Debug
SET OT_LIBR=x64\Release

SET OT_TEST=test
SET OT_LIBTESTD=\x64\DebugTest
SET OT_LIBTESTR=\x64\ReleaseTest

REM #########################################################################################################################################################################################################################################################################################################################################################################################
REM OpenTwin services root paths
SET OT_AUTHORISATION_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\AuthorisationService
SET OT_MODEL_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\Model
SET OT_GLOBAL_SESSION_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\GlobalSessionService
SET OT_LOCAL_SESSION_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\LocalSessionService
SET OT_GLOBAL_DIRECTORY_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\GlobalDirectoryService
SET OT_LOCAL_DIRECTORY_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\LocalDirectoryService
SET OT_RELAY_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\RelayService
SET OT_LOGGER_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\LoggerService
SET OT_PHREEC_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\PHREECService
SET OT_KRIGING_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\KrigingService
SET OT_MODELING_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\ModelingService
SET OT_VISUALIZATION_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\VisualizationService
SET OT_FITTD_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\FITTDService
SET OT_CARTESIAN_MESH_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\CartesianMeshService
SET OT_TET_MESH_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\TetMeshService
SET OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\ImportParameterizedData
SET OT_GETDP_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\GetDPService
SET OT_UI_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\uiService
SET OT_PYTHON_EXECUTION_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\PythonExecutionService
SET OT_PYTHON_EXECUTION_ROOT=%OPENTWIN_DEV_ROOT%\Services\PythonExecution
SET OT_BLOCKEDITORSERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\BlockEditorService
SET OT_DATA_PROCESSING_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\DataProcessingService
SET OT_CIRCUIT_SIMULATOR_SERVICE_ROOT=%OPENTWIN_DEV_ROOT%\Services\CircuitSimulatorService

REM #########################################################################################################################################################################################################################################################################################################################################################################################
REM OpenTwin library root paths
SET OT_CADMODELENTITIES_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\CadModelEntities
SET OT_DATASTORAGE_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\DataStorage
SET OT_MODELENTITIES_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\ModelEntities
SET OT_BLOCKENTITIES_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\BlockEntities

SET OT_COMMUNICATION_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\OTCommunication
SET OT_CORE_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\OTCore
SET OT_FOUNDATION_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\OTServiceFoundation
SET OT_SYSTEM_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\OTSystem
SET OT_GUI_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\OTGui
SET OT_WIDGETS_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\OTWidgets

SET OT_QWTWRAPPER_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\QwtWrapper
SET OT_RUBBERBANDAPI_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\RubberbandEngineCore
SET OT_RUBBERBAND_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\RubberbandEngineOsgWrapper
SET OT_UICORE_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\uiCore
SET OT_UIPLUGINAPI_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\UIPluginAPI
SET OT_VIEWER_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\Viewer

SET OT_RESULT_COLLECTION_ACCESS_ROOT=%OPENTWIN_DEV_ROOT%\Libraries\ResultCollectionAccess

REM #########################################################################################################################################################################################################################################################################################################################################################################################
REM OpenTwin tools root paths
SET OT_OTOOLKITAPI_ROOT=%OPENTWIN_DEV_ROOT%\Tools\OToolkitAPI
SET OT_OTOOLKIT_ROOT=%OPENTWIN_DEV_ROOT%\Tools\OToolkit

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

REM SET OT_DEFAULT_SERVICE_INCD=%OT_CORE_ROOT%\%OT_INC%;%OT_SYSTEM_ROOT%\%OT_INC%;%OT_COMMUNICATION_ROOT%\%OT_INC%;%OT_FOUNDATION_ROOT%\%OT_INC%;%OT_MODELENTITIES_ROOT%\%OT_INC%;%OT_DATASTORAGE_ROOT%\%OT_INC%;%OT_UIPLUGINAPI_ROOT%\%OT_INC%;%R_JSON_INCD%;%CURL_INCD%;%MONGO_C_ROOT%\Debug\include;%MONGO_CXX_ROOT%\Debug\include;%MONGO_BOOST_ROOT%
REM SET OT_DEFAULT_SERVICE_INCR=%OT_CORE_ROOT%\%OT_INC%;%OT_SYSTEM_ROOT%\%OT_INC%;%OT_COMMUNICATION_ROOT%\%OT_INC%;%OT_FOUNDATION_ROOT%\%OT_INC%;%OT_MODELENTITIES_ROOT%\%OT_INC%;%OT_DATASTORAGE_ROOT%\%OT_INC%;%OT_UIPLUGINAPI_ROOT%\%OT_INC%;%R_JSON_INCR%;%CURL_INCD%;%MONGO_C_ROOT%\Release\include;%MONGO_CXX_ROOT%\Release\include;%MONGO_BOOST_ROOT%
SET OT_DEFAULT_SERVICE_INCD=%OT_CORE_ROOT%\%OT_INC%;%OT_SYSTEM_ROOT%\%OT_INC%;%OT_COMMUNICATION_ROOT%\%OT_INC%;%OT_FOUNDATION_ROOT%\%OT_INC%;%OT_MODELENTITIES_ROOT%\%OT_INC%;%OT_DATASTORAGE_ROOT%\%OT_INC%;%OT_UIPLUGINAPI_ROOT%\%OT_INC%;%R_JSON_INCD%;%CURL_INCD%;%MONGO_C_INCD%;%MONGO_CXX_INCD%;%MONGO_BOOST_INCD%
SET OT_DEFAULT_SERVICE_INCR=%OT_CORE_ROOT%\%OT_INC%;%OT_SYSTEM_ROOT%\%OT_INC%;%OT_COMMUNICATION_ROOT%\%OT_INC%;%OT_FOUNDATION_ROOT%\%OT_INC%;%OT_MODELENTITIES_ROOT%\%OT_INC%;%OT_DATASTORAGE_ROOT%\%OT_INC%;%OT_UIPLUGINAPI_ROOT%\%OT_INC%;%R_JSON_INCR%;%CURL_INCR%;%MONGO_C_INCR%;%MONGO_CXX_INCR%;%MONGO_BOOST_INCR%

REM SET OT_DEFAULT_SERVICE_LIBPATHD=%OT_CORE_ROOT%\x64\Debug;%OT_SYSTEM_ROOT%\x64\Debug;%OT_COMMUNICATION_ROOT%\x64\Debug;%OT_FOUNDATION_ROOT%\x64\Debug;%OT_MODELENTITIES_ROOT%\x64\Debug;%OT_DATASTORAGE_ROOT%\x64\Debug;%OT_UIPLUGINAPI_ROOT%\x64\Debug;%CURL_LIB%;%MONGO_C_ROOT%\Debug\lib;%MONGO_CXX_ROOT%\Debug\lib;%ZLIB_LIB%\Debug\lib
REM SET OT_DEFAULT_SERVICE_LIBPATHR=%OT_CORE_ROOT%\x64\Release;%OT_SYSTEM_ROOT%\x64\Release;%OT_COMMUNICATION_ROOT%\x64\Release;%OT_FOUNDATION_ROOT%\x64\Release;%OT_MODELENTITIES_ROOT%\x64\Release;%OT_DATASTORAGE_ROOT%\x64\Release;%OT_UIPLUGINAPI_ROOT%\x64\Release;%CURL_LIB%;%MONGO_C_ROOT%\Release\lib;%MONGO_CXX_ROOT%\Release\lib;%ZLIB_LIB%\Release\lib
SET OT_DEFAULT_SERVICE_LIBPATHD=%OT_CORE_ROOT%\%OT_LIBD%;%OT_SYSTEM_ROOT%\%OT_LIBD%;%OT_COMMUNICATION_ROOT%\%OT_LIBD%;%OT_FOUNDATION_ROOT%\%OT_LIBD%;%OT_MODELENTITIES_ROOT%\%OT_LIBD%;%OT_DATASTORAGE_ROOT%\%OT_LIBD%;%OT_UIPLUGINAPI_ROOT%\%OT_LIBD%;%CURL_LIBPATHD%;%MONGO_C_LIBPATHD%;%MONGO_CXX_LIBPATHD%
SET OT_DEFAULT_SERVICE_LIBPATHR=%OT_CORE_ROOT%\%OT_LIBR%;%OT_SYSTEM_ROOT%\%OT_LIBR%;%OT_COMMUNICATION_ROOT%\%OT_LIBR%;%OT_FOUNDATION_ROOT%\%OT_LIBR%;%OT_MODELENTITIES_ROOT%\%OT_LIBR%;%OT_DATASTORAGE_ROOT%\%OT_LIBR%;%OT_UIPLUGINAPI_ROOT%\%OT_LIBR%;%CURL_LIBPATHR%;%MONGO_C_LIBPATHR%;%MONGO_CXX_LIBPATHR%

REM SET OT_DEFAULT_SERVICE_LIBD=OpenTwinCore.lib;OpenTwinSystem.lib;OpenTwinCommunication.lib;OpenTwinServiceFoundation.lib;ModelEntities.lib;DataStorage.lib;libCurl.lib;mongocxx.lib;bsoncxx.lib;mongoc-1.0.lib;bson-1.0.lib
REM SET OT_DEFAULT_SERVICE_LIBR=OpenTwinCore.lib;OpenTwinSystem.lib;OpenTwinCommunication.lib;OpenTwinServiceFoundation.lib;ModelEntities.lib;DataStorage.lib;libCurl.lib;mongocxx.lib;bsoncxx.lib;mongoc-1.0.lib;bson-1.0.lib
SET OT_DEFAULT_SERVICE_LIBD=OTCore.lib;OTSystem.lib;OTCommunication.lib;OTServiceFoundation.lib;ModelEntities.lib;DataStorage.lib;UIPluginAPI.lib;%CURL_LIBD%;%MONGO_C_LIBD%;%MONGO_CXX_LIBD%
SET OT_DEFAULT_SERVICE_LIBR=OTCore.lib;OTSystem.lib;OTCommunication.lib;OTServiceFoundation.lib;ModelEntities.lib;DataStorage.lib;UIPluginAPI.lib;%CURL_LIBR%;%MONGO_C_LIBR%;%MONGO_CXX_LIBR%

REM SET OT_DEFAULT_SERVICE_DLLD=%OT_CORE_ROOT%\x64\Debug;%OT_SYSTEM_ROOT%\x64\Debug;%OT_COMMUNICATION_ROOT%\x64\Debug;%OT_FOUNDATION_ROOT%\x64\Debug;%OT_MODELENTITIES_ROOT%\x64\Debug;%OT_DATASTORAGE_ROOT%\x64\Debug;%OT_UIPLUGINAPI_ROOT%\x64\Debug;%CURL_DLLD%;%MONGO_C_ROOT%\Debug\bin;%MONGO_CXX_ROOT%\Debug\bin
REM SET OT_DEFAULT_SERVICE_DLLR=%OT_CORE_ROOT%\x64\Release;%OT_SYSTEM_ROOT%\x64\Release;%OT_COMMUNICATION_ROOT%\x64\Release;%OT_FOUNDATION_ROOT%\x64\Release;%OT_MODELENTITIES_ROOT%\x64\Release;%OT_DATASTORAGE_ROOT%\x64\Release;%OT_UIPLUGINAPI_ROOT%\x64\Release;%CURL_DLL%;%MONGO_C_ROOT%\Release\bin;%MONGO_CXX_ROOT%\Release\bin
SET OT_DEFAULT_SERVICE_DLLD=%OT_CORE_ROOT%\%OT_DLLD%;%OT_SYSTEM_ROOT%\%OT_DLLD%;%OT_COMMUNICATION_ROOT%\%OT_DLLD%;%OT_FOUNDATION_ROOT%\%OT_DLLD%;%OT_MODELENTITIES_ROOT%\%OT_DLLD%;%OT_DATASTORAGE_ROOT%\%OT_DLLD%;%OT_UIPLUGINAPI_ROOT%\%OT_DLLD%;%CURL_DLLD%;%MONGO_C_DLLD%;%MONGO_CXX_DLLD%;%CURL_DLLD%;%ZLIB_DLLPATHD%;%VTK_DLLD%;%OSG_DLLD%;%QT_DLLD%;%QWT_LIB_DLLD%;%QWTPOLAR_LIB_DLLD%;%QT_TT_DLLD%
SET OT_DEFAULT_SERVICE_DLLR=%OT_CORE_ROOT%\%OT_DLLR%;%OT_SYSTEM_ROOT%\%OT_DLLR%;%OT_COMMUNICATION_ROOT%\%OT_DLLR%;%OT_FOUNDATION_ROOT%\%OT_DLLR%;%OT_MODELENTITIES_ROOT%\%OT_DLLR%;%OT_DATASTORAGE_ROOT%\%OT_DLLR%;%OT_UIPLUGINAPI_ROOT%\%OT_DLLR%;%CURL_DLLR%;%MONGO_C_DLLR%;%MONGO_CXX_DLLR%;%CURL_DLLR%;%ZLIB_DLLPATHR%;%VTK_DLLR%;%OSG_DLLR%;%QT_DLLR%;%QWT_LIB_DLLR%;%QWTPOLAR_LIB_DLLR%;%QT_TT_DLLR%

REM #########################################################################################################################################################################################################################################################################################################################################################################################
REM OpenTwin all services path

SET OT_ALL_DLLD=%OT_RESULT_COLLECTION_ACCESS_ROOT%\%OT_DLLD%;%OT_BLOCKENTITIES_ROOT%\%OT_DLLD%;%OT_BLOCKEDITOR_ROOT%\%OT_DLLD%;%OT_BLOCKEDITORAPI_ROOT%\%OT_DLLD%;%OT_CADMODELENTITIES_ROOT%\%OT_DLLD%;%OT_DATASTORAGE_ROOT%\%OT_DLLD%;%OT_MODELENTITIES_ROOT%\%OT_DLLD%;%OT_COMMUNICATION_ROOT%\%OT_DLLD%;%OT_CORE_ROOT%\%OT_DLLD%;%OT_FOUNDATION_ROOT%\%OT_DLLD%;%OT_SYSTEM_ROOT%\%OT_DLLD%;%OT_GUI_ROOT%\%OT_DLLD%;%OT_WIDGETS_ROOT%\%OT_DLLD%;%OT_QWTWRAPPER_ROOT%\%OT_DLLD%;%OT_RUBBERBANDAPI_ROOT%\%OT_DLLD%;%OT_RUBBERBAND_ROOT%\%OT_DLLD%;%OT_UICORE_ROOT%\%OT_DLLD%;%OT_UIPLUGINAPI_ROOT%\%OT_DLLD%;%OT_VIEWER_ROOT%\%OT_DLLD%;%MONGO_C_DLLD%;%MONGO_CXX_DLLD%;%CURL_DLLD%;%ZLIB_DLLPATHD%;%VTK_DLLD%;%OSG_DLLD%;%QT_DLLD%;%QWT_LIB_DLLD%;%QWTPOLAR_LIB_DLLD%;%QT_TT_DLLD%;%PYTHON310_ROOT%
SET OT_ALL_DLLR=%OT_RESULT_COLLECTION_ACCESS_ROOT%\%OT_DLLR%;%OT_BLOCKENTITIES_ROOT%\%OT_DLLR%;%OT_BLOCKEDITOR_ROOT%\%OT_DLLR%;%OT_BLOCKEDITORAPI_ROOT%\%OT_DLLR%;%OT_CADMODELENTITIES_ROOT%\%OT_DLLR%;%OT_DATASTORAGE_ROOT%\%OT_DLLR%;%OT_MODELENTITIES_ROOT%\%OT_DLLR%;%OT_COMMUNICATION_ROOT%\%OT_DLLR%;%OT_CORE_ROOT%\%OT_DLLR%;%OT_FOUNDATION_ROOT%\%OT_DLLR%;%OT_SYSTEM_ROOT%\%OT_DLLR%;%OT_GUI_ROOT%\%OT_DLLR%;%OT_WIDGETS_ROOT%\%OT_DLLR%;%OT_QWTWRAPPER_ROOT%\%OT_DLLR%;%OT_RUBBERBANDAPI_ROOT%\%OT_DLLR%;%OT_RUBBERBAND_ROOT%\%OT_DLLR%;%OT_UICORE_ROOT%\%OT_DLLR%;%OT_UIPLUGINAPI_ROOT%\%OT_DLLR%;%OT_VIEWER_ROOT%\%OT_DLLR%;%MONGO_C_DLLR%;%MONGO_CXX_DLLR%;%CURL_DLLR%;%ZLIB_DLLPATHR%;%VTK_DLLR%;%OSG_DLLR%;%QT_DLLR%;%QWT_LIB_DLLR%;%QWTPOLAR_LIB_DLLR%;%QT_TT_DLLR%;%PYTHON310_ROOT%

REM #########################################################################################################################################################################################################################################################################################################################################################################################

REM Add Deployment path to the path
SET PATH=%OPENTWIN_DEV_ROOT%\Deployment;%PATH%

REM Set the env defined at the end to ensure everything else was completed successfully
SET OPENTWIN_DEV_ENV_DEFINED=1
ECHO OpenTwin Developer environment was set up successfully.

GOTO END

REM In case we want the user to see the messages before closing the window
:PAUSE_END
pause
GOTO END

:END
