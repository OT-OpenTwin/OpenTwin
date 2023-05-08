@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 3. DEVENV_ROOT_2022
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

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)

REM Clean up the Deployment directory

RMDIR /S /Q "%SIM_PLAT_ROOT%\Deployment\Frontend"
MKDIR "%SIM_PLAT_ROOT%\Deployment\Frontend"

RMDIR /S /Q "%SIM_PLAT_ROOT%\Deployment\Frontend-Installer"
MKDIR "%SIM_PLAT_ROOT%\Deployment\Frontend-Installer"

REM ===========================================================================
REM Copy the library files
REM ===========================================================================

REM Qwt
COPY "%QWT_LIB_ROOT%\lib\qwt.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%QWT_POLAR_LIB_ROOT%\lib\qwtpolar.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM QtTabToolbar
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\QtTabToolbar\src\TabToolbar\Release\TabToolbar.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM OpenSceneGraph
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenSceneGraph\OpenSceneGraph-OpenSceneGraph-3.6.3\bin\Release\*.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

MKDIR "%SIM_PLAT_ROOT%\Deployment\Frontend\fonts"
XCOPY /S "%SIM_PLAT_ROOT%\Libraries\Fonts" "%SIM_PLAT_ROOT%\Deployment\Frontend\fonts"

MKDIR "%SIM_PLAT_ROOT%\Deployment\Frontend\osgPlugins-3.6.3"
XCOPY /S "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenSceneGraph\OpenSceneGraph-OpenSceneGraph-3.6.3\bin\Release\osgPlugins-3.6.3" "%SIM_PLAT_ROOT%\Deployment\Frontend\osgPlugins-3.6.3"

REM Qt
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\libEGL.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\libGLESv2.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\opengl32sw.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DAnimation.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DCore.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DExtras.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DInput.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DLogic.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuick.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuickAnimation.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuickExtras.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuickInput.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuickRender.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DQuickScene2D.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt53DRender.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Bluetooth.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Charts.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Concurrent.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Core.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5DataVisualization.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5DBus.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Gamepad.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Gui.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Help.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Location.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Multimedia.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5MultimediaQuick.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5MultimediaWidgets.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Network.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5NetworkAuth.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Nfc.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5OpenGL.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Positioning.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5PositioningQuick.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5PrintSupport.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Purchasing.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Qml.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Quick.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5QuickControls2.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5QuickParticles.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5QuickTemplates2.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5QuickTest.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5QuickWidgets.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5RemoteObjects.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Script.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5ScriptTools.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Scxml.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Sensors.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5SerialBus.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5SerialPort.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Sql.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Svg.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Test.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5TextToSpeech.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebChannel.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebEngine.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
REM COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebEngineCore.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebEngineWidgets.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebSockets.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WebView.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Widgets.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5WinExtras.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5Xml.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\bin\Qt5XmlPatterns.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

MKDIR "%SIM_PLAT_ROOT%\Deployment\Frontend\plugins"
MKDIR "%SIM_PLAT_ROOT%\Deployment\Frontend\plugins\platforms"
MKDIR "%SIM_PLAT_ROOT%\Deployment\Frontend\plugins\platformthemes"
MKDIR "%SIM_PLAT_ROOT%\Deployment\Frontend\plugins\styles"
MKDIR "%SIM_PLAT_ROOT%\Deployment\Frontend\plugins\imageformats"
XCOPY /S "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\plugins\platforms\*.dll"      "%SIM_PLAT_ROOT%\Deployment\Frontend\plugins\platforms"
XCOPY /S "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\plugins\platformthemes\*.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend\plugins\platformthemes"
XCOPY /S "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\plugins\styles\*.dll"         "%SIM_PLAT_ROOT%\Deployment\Frontend\plugins\styles"
XCOPY /S "%SIM_PLAT_ROOT%\Third_Party_Libraries\Qt\5.11.2\msvc2017_64\plugins\imageformats\*.dll"   "%SIM_PLAT_ROOT%\Deployment\Frontend\plugins\imageformats"

REM Copy the OpenSSL libs for the Qt Websocket

COPY /Y "%SIM_PLAT_ROOT%\Third_Party_Libraries\OpenSSL\LibsForQtWebsocket\*.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM Copy the icons

MKDIR "%SIM_PLAT_ROOT%\Deployment\Frontend\icons"
XCOPY /S "%SIM_PLAT_ROOT%\Libraries\Icons" "%SIM_PLAT_ROOT%\Deployment\Frontend\icons"

REM CURL

COPY "%CURL_DLL%\libcurl.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM OPENSSL

COPY "%OPENSSL_DLL%\libcrypto-1_1-x64.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%OPENSSL_DLL%\libssl-1_1-x64.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM MongoDB

COPY /Y "%MONGO_CXX_ROOT%\Release\bin\*.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY /Y "%MONGO_C_ROOT%\Release\bin\*.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM ZLIB

COPY "%ZLIB_LIB%\zlibwapi.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM ===========================================================================
REM Copy the build files
REM ===========================================================================

REM Library OpenTwinCommunication
COPY "%SIM_PLAT_ROOT%\Libraries\OpenTwinCommunication\x64\Release\OpenTwinCommunication.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM Library OpenTwinServiceFoundation
COPY "%SIM_PLAT_ROOT%\Libraries\OpenTwinServiceFoundation\x64\Release\OpenTwinServiceFoundation.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM Library OpenTwinLogger
COPY "%SIM_PLAT_ROOT%\Libraries\OpenTwinLogger\x64\Release\OpenTwinLogger.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM Library ModelEntities
COPY "%SIM_PLAT_ROOT%\Libraries\ModelEntities\x64\Release\ModelEntities.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM Library Viewer
COPY "%SIM_PLAT_ROOT%\Libraries\Viewer\x64\Release\Viewer.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM Library DataStorage
COPY "%SIM_PLAT_ROOT%\Libraries\DataStorage\x64\Release\DataStorage.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM UI Core
COPY "%UICORE_LIB_ROOT%\x64\Release\uiCore.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM Rubberband 
COPY "%RUBBERBAND_ENGINE_CORE%\x64\Release\RubberbandEngineCore.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"
COPY "%RUBBERBAND_ENGINE_OSG%\x64\Release\RubberbandOsgWrapper.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM UI Service
COPY "%UISERVICE_LIB_ROOT%\x64\Release\uiService.dll" "%SIM_PLAT_ROOT%\Deployment\Frontend"

REM UI Frontend
COPY "%UISERVICE_LIB_ROOT%\x64\ExeRelease\uiFrontend.exe" "%SIM_PLAT_ROOT%\Deployment\Frontend\OpenTwin.exe"

REM Copy the certificates
MKDIR "%SIM_PLAT_ROOT%\Deployment\Frontend\Certificates"
COPY "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca.pem" "%SIM_PLAT_ROOT%\Deployment\Frontend\Certificates"

ECHO [Paths] > "%SIM_PLAT_ROOT%\Deployment\Frontend\qt.conf"
ECHO Plugins = .\\plugins >> "%SIM_PLAT_ROOT%\Deployment\Frontend\qt.conf"

REM ===========================================================================
REM Finally create the installer
REM ===========================================================================

CALL "C:\Program Files (x86)\NSIS\makensis.exe" Install-OpenTwin.nsi

MOVE "Install OpenTwin.exe" ..\Deployment\Frontend-Installer"

