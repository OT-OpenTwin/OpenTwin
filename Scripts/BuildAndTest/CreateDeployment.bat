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

IF NOT DEFINED OPEN_TWIN_AUTH_PORT (
	SET OPEN_TWIN_AUTH_PORT=8092
)

IF NOT DEFINED OPEN_TWIN_LSS_PORT (
	SET OPEN_TWIN_LSS_PORT=8093
)

IF NOT DEFINED OPEN_TWIN_GSS_PORT (
	SET OPEN_TWIN_GSS_PORT=8091
)

REM Shutdown microservices
taskkill /IM "open_twin.exe" /F

REM Shutdown ui
taskkill /IM "uiFrontend.exe" /F

REM Shutdown Apache server
taskkill /IM "httpd.exe" /F

REM Clean up the Deployment directory

RMDIR /S /Q "%OT_DEPLOYMENT_DIR%"
MKDIR "%OT_DEPLOYMENT_DIR%"
MKDIR "%OT_DEPLOYMENT_DIR%\uiPlugins"


REM ===========================================================================
REM Copy the library files
REM ===========================================================================
	
REM Qwt
COPY "%QWT_LIB_ROOT%\lib\qwt.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QWT_POLAR_LIB_ROOT%\lib\qwtpolar.dll" "%OT_DEPLOYMENT_DIR%"

REM QtTabToolbar
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\QtTabToolbar\src\TabToolbar\Release\TabToolbar.dll" "%OT_DEPLOYMENT_DIR%"

REM OpenSceneGraph
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenSceneGraph\OpenSceneGraph-OpenSceneGraph-3.6.3\bin\Release\*.dll" "%OT_DEPLOYMENT_DIR%"

MKDIR "%OT_DEPLOYMENT_DIR%\fonts"
XCOPY /S "%SIM_PLAT_ROOT%\Libraries\Fonts" "%OT_DEPLOYMENT_DIR%\fonts"

MKDIR "%OT_DEPLOYMENT_DIR%\osgPlugins-3.6.3"
XCOPY /S "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenSceneGraph\OpenSceneGraph-OpenSceneGraph-3.6.3\bin\Release\osgPlugins-3.6.3" "%OT_DEPLOYMENT_DIR%\osgPlugins-3.6.3"

REM Qt
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\libEGL.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\libGLESv2.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\opengl32sw.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DAnimation.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DCore.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DExtras.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DInput.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DLogic.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuickAnimation.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuickExtras.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuickInput.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuickRender.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuickScene2D.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DRender.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Bluetooth.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Charts.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Concurrent.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Core.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5DataVisualization.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5DBus.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Gamepad.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Gui.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Help.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Location.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Multimedia.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5MultimediaQuick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5MultimediaWidgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Network.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5NetworkAuth.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Nfc.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5OpenGL.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Positioning.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5PositioningQuick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5PrintSupport.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Purchasing.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Qml.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Quick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5QuickControls2.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5QuickParticles.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5QuickTemplates2.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5QuickTest.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5QuickWidgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5RemoteObjects.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Script.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5ScriptTools.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Scxml.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Sensors.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5SerialBus.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5SerialPort.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Sql.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Svg.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Test.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5TextToSpeech.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebChannel.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebEngine.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebEngineCore.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebEngineWidgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebSockets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebView.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Widgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WinExtras.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Xml.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5XmlPatterns.dll" "%OT_DEPLOYMENT_DIR%"

MKDIR "%OT_DEPLOYMENT_DIR%\plugins"
XCOPY /S "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\plugins" "%OT_DEPLOYMENT_DIR%\plugins"

REM Copy the OpenSSL libs for the Qt Websocket

COPY /Y "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenSSL\LibsForQtWebsocket\*.dll" "%OT_DEPLOYMENT_DIR%"

REM Boost
COPY "%BOOST_LIB%\*.dll" "%OT_DEPLOYMENT_DIR%"

REM CGAL
COPY "%GMP_LIB%\*.dll" "%OT_DEPLOYMENT_DIR%"

REM OpenCascade
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenCASCADE-7.5.0-vc14-64\ffmpeg-3.3.4-64\bin\*.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenCASCADE-7.5.0-vc14-64\freeimage-3.17.0-vc14-64\bin\*.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenCASCADE-7.5.0-vc14-64\freetype-2.5.5-vc14-64\bin\*.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenCASCADE-7.5.0-vc14-64\opencascade-7.5.0\win64\vc14\bin\*.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenCASCADE-7.5.0-vc14-64\tbb_2017.0.100\bin\intel64\vc14\tbb.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenCASCADE-7.5.0-vc14-64\tbb_2017.0.100\bin\intel64\vc14\tbb_preview.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenCASCADE-7.5.0-vc14-64\tbb_2017.0.100\bin\intel64\vc14\tbbmalloc.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenCASCADE-7.5.0-vc14-64\openvr-1.14.15-64\bin\win64\*.dll" "%OT_DEPLOYMENT_DIR%"

REM VTK
COPY "%VTK_ROOT%\build_Win64\bin\Release\*.dll" "%OT_DEPLOYMENT_DIR%"

REM Copy the icons

MKDIR "%OT_DEPLOYMENT_DIR%\icons"
XCOPY /S "%SIM_PLAT_ROOT%\Libraries\Icons" "%OT_DEPLOYMENT_DIR%\icons"

REM GMSH

COPY "%GMSH_ROOT_BIN%\gmsh.dll" "%OT_DEPLOYMENT_DIR%"

REM CURL

COPY "%CURL_DLL%\libcurl.dll" "%OT_DEPLOYMENT_DIR%"

REM OPENSSL

COPY "%OPENSSL_DLL%\libcrypto-1_1-x64.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%OPENSSL_DLL%\libssl-1_1-x64.dll" "%OT_DEPLOYMENT_DIR%"

REM MongoDB

COPY /Y "%MONGO_CXX_ROOT%\Release\bin\*.dll" "%OT_DEPLOYMENT_DIR%"
COPY /Y "%MONGO_C_ROOT%\Release\bin\*.dll" "%OT_DEPLOYMENT_DIR%"

REM ZLIB

COPY "%ZLIB_LIB%\zlibwapi.dll" "%OT_DEPLOYMENT_DIR%"

REM EMBREE

COPY "%EMBREE_BIN%\embree3.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%EMBREE_BIN%\tbb12.dll" "%OT_DEPLOYMENT_DIR%"

REM GETDP

COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\getdp\x64\*.*" "%OT_DEPLOYMENT_DIR%"

REM PYTHON
MKDIR "%OT_DEPLOYMENT_DIR%\Python"
XCOPY /S "%SIM_PLAT_ROOT%\Third_Party_Libraries\Python\python-3.9.5.amd64\*.*" "%OT_DEPLOYMENT_DIR%\Python"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Python\python-3.9.5.amd64\python39.dll" "%OT_DEPLOYMENT_DIR%"

REM Visual Studio Redistributables
MKDIR "%OT_DEPLOYMENT_DIR%\VC_Redist"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\VisualStudioRuntime\VS_2017\VC_redist.x86.exe" "%OT_DEPLOYMENT_DIR%\VC_Redist"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\VisualStudioRuntime\VS_2017\VC_redist.x64.exe" "%OT_DEPLOYMENT_DIR%\VC_Redist"

REM Apache Server
MKDIR "%OT_DEPLOYMENT_DIR%\Apache"
XCOPY /S "%SIM_PLAT_ROOT%\Third_Party_Libraries\Apache\Apache24_windows\*.*" "%OT_DEPLOYMENT_DIR%\Apache"

REM ===========================================================================
REM Copy the build files
REM ===========================================================================

CALL "%SIM_PLAT_ROOT%\MasterBuild\updateDeploymentLibrariesOnly.bat"

REM Python Microservice 
REM COPY "%SIM_PLAT_ROOT%\Microservices\FlaskFramework\dist\FlaskFramework.exe" "%OT_DEPLOYMENT_DIR%"

REM Microservices Launcher
XCOPY /S "%SIM_PLAT_ROOT%\Microservices\Launcher\*.*" "%OT_DEPLOYMENT_DIR%"

REM Copy the Password Encryption Tool
XCOPY /S "%SIM_PLAT_ROOT%\Microservices\PasswordEncryption\x64\Release\PasswordEncryption.exe" "%OT_DEPLOYMENT_DIR%"

REM Copy the certificate creation tools
XCOPY /S "%OPENTWIN_THIRDPARTY_ROOT%\CertificateCreation\*.*" "%OT_DEPLOYMENT_DIR%\Certificates"
XCOPY /S "%OPENTWIN_DEV_ROOT%\Certificates\CreateServerCertificates\*.*" "%OT_DEPLOYMENT_DIR%\Certificates"

REM Copy the certificates
COPY "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca.pem" "%OT_DEPLOYMENT_DIR%\Certificates"
COPY "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server.pem" "%OT_DEPLOYMENT_DIR%\Certificates"
COPY "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server-key.pem" "%OT_DEPLOYMENT_DIR%\Certificates"
COPY "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca-key.pem" "%OT_DEPLOYMENT_DIR%\Certificates"
COPY "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\certificateKeyFile.pem" "%OT_DEPLOYMENT_DIR%\Certificates"

ECHO [Paths] > "%OT_DEPLOYMENT_DIR%\qt.conf"
ECHO Plugins = .\\plugins >> "%OT_DEPLOYMENT_DIR%\qt.conf"

