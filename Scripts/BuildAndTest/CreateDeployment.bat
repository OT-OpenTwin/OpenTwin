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
MKDIR "%OT_DEPLOYMENT_DIR%\Python"

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
REM COPY "%QT_DLLR%\libEGL.dll" "%OT_DEPLOYMENT_DIR%"
REM COPY "%QT_DLLR%\libGLESv2.dll" "%OT_DEPLOYMENT_DIR%"
REM COPY "%QT_DLLR%\opengl32sw.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DAnimation.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DCore.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DExtras.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DInput.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DLogic.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DQuick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DQuickAnimation.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DQuickExtras.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DQuickInput.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DQuickRender.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DQuickScene2D.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt63DRender.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Bluetooth.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Charts.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Concurrent.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Core.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6DataVisualization.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6DBus.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Gamepad.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Gui.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Help.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Location.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Multimedia.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6MultimediaQuick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6MultimediaWidgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Network.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6NetworkAuth.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Nfc.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6OpenGL.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6OpenGLWidgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Positioning.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6PositioningQuick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6PrintSupport.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Purchasing.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Qml.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Quick.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6QuickControls2.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6QuickParticles.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6QuickTemplates2.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6QuickTest.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6QuickWidgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6RemoteObjects.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Script.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6ScriptTools.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Scxml.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Sensors.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6SerialBus.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6SerialPort.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Sql.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Svg.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Test.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6TextToSpeech.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6WebChannel.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6WebEngine.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6WebEngineCore.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6WebEngineWidgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6WebSockets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6WebView.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Widgets.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6WinExtras.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6Xml.dll" "%OT_DEPLOYMENT_DIR%"
COPY "%QT_DLLR%\Qt6XmlPatterns.dll" "%OT_DEPLOYMENT_DIR%"

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

REM ELMERFEM
MKDIR "%OT_DEPLOYMENT_DIR%\ElmerFEM"
XCOPY /S "%ELMERFEM_BIN%\*.*" "%OT_DEPLOYMENT_DIR%\ElmerFEM"

REM PYTHON
COPY "%PYTHON_ROOT%\python.exe" "%OT_DEPLOYMENT_DIR%"
COPY "%PYTHON_ROOT%\python310.dll" "%OT_DEPLOYMENT_DIR%"
XCOPY /S "%PYTHON_ROOT%\Lib" "%OT_DEPLOYMENT_DIR%\Python" 

REM NGSpice
COPY "%NGSPICE_ROOT%\visualc\sharedspice\Release.x64\ngspice.dll" "%OT_DEPLOYMENT_DIR%"

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

GOTO END

:PAUSE_END
pause
GOTO END

:END
