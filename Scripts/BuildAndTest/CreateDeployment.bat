@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 3. DEVENV_ROOT_2022

IF "%OPENTWIN_DEV_ROOT%"=="" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto END
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto PAUSE_END
)

IF "%DEVENV_ROOT_2022%"=="" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto END
)

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

IF "%OT_DEPLOYMENT_DIR%" == "" (
    SET OT_DEPLOYMENT_DIR="%OPENTWIN_DEV_ROOT%\Deployment"
)

REM ===========================================================================
REM Shutdown the session and authorisation services if they are still running 
REM ===========================================================================

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
MKDIR "%OT_DEPLOYMENT_DIR%\Certificates"


REM ===========================================================================
REM Copy the library files
REM ===========================================================================
	
REM Qwt
COPY "%QWT_LIB_DLLR%\qwt.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QWTPOLAR_LIB_DLLR%\qwtpolar.dll" "%OT_DEPLOYMENT_DIR%"

REM QtTabToolbar
COPY "%QT_TT_DLLR%\TabToolbar.dll" "%OT_DEPLOYMENT_DIR%"

REM OpenSceneGraph
COPY "%OSG_DLLR%\*.dll" "%OT_DEPLOYMENT_DIR%"
MKDIR "%OT_DEPLOYMENT_DIR%\osgPlugins-3.6.3"
XCOPY /S "%OSG_DLLR%\osgPlugins-3.6.3" "%OT_DEPLOYMENT_DIR%\osgPlugins-3.6.3"

REM Fonts
MKDIR "%OT_DEPLOYMENT_DIR%\fonts"
XCOPY /S "%OPENTWIN_DEV_ROOT%\Assets\Fonts" "%OT_DEPLOYMENT_DIR%\fonts"

REM Qt
COPY "%QT_DLLR%\libEGL.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\libGLESv2.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\opengl32sw.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DAnimation.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DCore.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DExtras.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DInput.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DLogic.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DQuick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DQuickAnimation.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DQuickExtras.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DQuickInput.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DQuickRender.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DQuickScene2D.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt53DRender.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Bluetooth.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Charts.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Concurrent.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Core.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5DataVisualization.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5DBus.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Gamepad.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Gui.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Help.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Location.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Multimedia.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5MultimediaQuick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5MultimediaWidgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Network.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5NetworkAuth.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Nfc.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5OpenGL.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Positioning.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5PositioningQuick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5PrintSupport.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Purchasing.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Qml.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Quick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5QuickControls2.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5QuickParticles.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5QuickTemplates2.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5QuickTest.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5QuickWidgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5RemoteObjects.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Script.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5ScriptTools.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Scxml.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Sensors.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5SerialBus.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5SerialPort.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Sql.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Svg.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Test.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5TextToSpeech.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5WebChannel.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5WebEngine.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5WebEngineCore.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5WebEngineWidgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5WebSockets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5WebView.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Widgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5WinExtras.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5Xml.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt5XmlPatterns.dll" "%OT_DEPLOYMENT_DIR%"

MKDIR "%OT_DEPLOYMENT_DIR%\plugins"
XCOPY /S "%QDIR%\plugins" "%OT_DEPLOYMENT_DIR%\plugins"

REM Copy the OpenSSL libs for the Qt Websocket
COPY /Y "%OPENSSL_WEBSOCKET_DLLR%\*.dll" "%OT_DEPLOYMENT_DIR%"

REM Boost
COPY "%BOOST_DLLPATHR%\*.dll" "%OT_DEPLOYMENT_DIR%"

REM CGAL
COPY "%GMP_DLLPATHR%\*.dll" "%OT_DEPLOYMENT_DIR%"

REM OpenCascade
COPY "%FMP_DLLR%\*.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%FRI_DLLR%\*.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%FRT_DLLR%\*.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%OC_DLLR%\*.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%TBB_DLLR%\tbb.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%TBB_DLLR%\tbb_preview.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%TBB_DLLR%\tbbmalloc.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%OVR_DLLR%\*.dll" "%OT_DEPLOYMENT_DIR%"

REM VTK
COPY "%VTK_DLLR%\*.dll" "%OT_DEPLOYMENT_DIR%"

REM Copy the icons
MKDIR "%OT_DEPLOYMENT_DIR%\icons"
XCOPY /S "%OPENTWIN_DEV_ROOT%\Assets\Icons" "%OT_DEPLOYMENT_DIR%\icons"

REM GMSH
COPY "%GMSH_ROOT_BIN%\gmsh.dll" "%OT_DEPLOYMENT_DIR%"

REM CURL
COPY "%CURL_DLLR%\libcurl.dll" "%OT_DEPLOYMENT_DIR%"

REM OPENSSL
COPY "%OPENSSL_DLL%\libcrypto-1_1-x64.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%OPENSSL_DLL%\libssl-1_1-x64.dll" "%OT_DEPLOYMENT_DIR%"

REM MongoDB
COPY /Y "%MONGO_CXX_DLLR%\*.dll" "%OT_DEPLOYMENT_DIR%"
COPY /Y "%MONGO_C_DLLR%\*.dll" "%OT_DEPLOYMENT_DIR%"

REM ZLIB
COPY "%ZLIB_DLLPATHR%\zlib.dll" "%OT_DEPLOYMENT_DIR%"

REM EMBREE
COPY "%EMBREE_BIN%\embree3.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%EMBREE_BIN%\tbb12.dll" "%OT_DEPLOYMENT_DIR%"

REM GETDP
COPY "%GETDP_BIN%\*.*" "%OT_DEPLOYMENT_DIR%"

REM PYTHON
MKDIR "%OT_DEPLOYMENT_DIR%\Python"
XCOPY /S "%PYTHON_ROOT%\*.*" "%OT_DEPLOYMENT_DIR%\Python"
COPY "%PYTHON_ROOT%\python39.dll" "%OT_DEPLOYMENT_DIR%"

REM Visual Studio Redistributables
MKDIR "%OT_DEPLOYMENT_DIR%\VC_Redist"
COPY "%VC_REDIST_ROOT%\*.exe" "%OT_DEPLOYMENT_DIR%\VC_Redist"

REM Apache Server
MKDIR "%OT_DEPLOYMENT_DIR%\Apache"
XCOPY /S "%APACHE_ROOT%\*.*" "%OT_DEPLOYMENT_DIR%\Apache"

REM ===========================================================================
REM Copy the build files
REM ===========================================================================

CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\UpdateDeploymentLibrariesOnly.bat"

REM Launcher scripts
XCOPY /S "%OPENTWIN_DEV_ROOT%\Scripts\Launcher\*.*" "%OT_DEPLOYMENT_DIR%"

REM Copy the Password Encryption Tool
XCOPY /S "%OPENTWIN_DEV_ROOT%\Tools\PasswordEncryption\x64\Release\PasswordEncryption.exe" "%OT_DEPLOYMENT_DIR%"

REM Copy the certificate creation tools
XCOPY /S "%OPENTWIN_THIRDPARTY_ROOT%\CertificateCreation\*.*" "%OT_DEPLOYMENT_DIR%\Certificates"
XCOPY /S "%OPENTWIN_DEV_ROOT%\Certificates\CreateServerCertificates\*.*" "%OT_DEPLOYMENT_DIR%\Certificates"

REM Copy the certificates
COPY "%OPENTWIN_DEV_ROOT%\Certificates\Generated\ca.pem" "%OT_DEPLOYMENT_DIR%\Certificates"
COPY "%OPENTWIN_DEV_ROOT%\Certificates\Generated\server.pem" "%OT_DEPLOYMENT_DIR%\Certificates"
COPY "%OPENTWIN_DEV_ROOT%\Certificates\Generated\server-key.pem" "%OT_DEPLOYMENT_DIR%\Certificates"
COPY "%OPENTWIN_DEV_ROOT%\Certificates\Generated\ca-key.pem" "%OT_DEPLOYMENT_DIR%\Certificates"
COPY "%OPENTWIN_DEV_ROOT%\Certificates\Generated\certificateKeyFile.pem" "%OT_DEPLOYMENT_DIR%\Certificates"

ECHO [Paths] > "%OT_DEPLOYMENT_DIR%\qt.conf"
ECHO Plugins = .\\plugins >> "%OT_DEPLOYMENT_DIR%\qt.conf"

