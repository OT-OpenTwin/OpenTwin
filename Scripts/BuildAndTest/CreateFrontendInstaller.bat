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
SET OPENTWIN_FRONTEND_DEPLOYMENT=%OPENTWIN_DEV_ROOT%\Deployment_Frontend
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Clean up the FrontendDeployment directory
RMDIR /S /Q "%OPENTWIN_FRONTEND_DEPLOYMENT%"
MKDIR 		"%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM create FrontendInstaller folder in Deployment
RMDIR /S /Q "%OPENTWIN_DEV_ROOT%\Deployment\FrontendInstaller"
MKDIR 		"%OPENTWIN_DEV_ROOT%\Deployment\FrontendInstaller"

REM ==========================================
REM System files
REM ==========================================

REM This files are needed for the distribution of the frontend

ECHO.
ECHO ==========================================
ECHO 	Copy System files
ECHO ==========================================

SET SYSTEM_32=%SYSTEMROOT%\System32
COPY %SYSTEM_32%\downlevel\api-ms-win-crt-runtime-l1-1-0.dll %OPENTWIN_FRONTEND_DEPLOYMENT%
COPY %SYSTEM_32%\msvcp140.dll 					      		 %OPENTWIN_FRONTEND_DEPLOYMENT%
COPY %SYSTEM_32%\msvcp140_1.dll 					 		 %OPENTWIN_FRONTEND_DEPLOYMENT%
COPY %SYSTEM_32%\msvcp140_2.dll 					 		 %OPENTWIN_FRONTEND_DEPLOYMENT%
COPY %SYSTEM_32%\ucrtbase.dll 				 	 			 %OPENTWIN_FRONTEND_DEPLOYMENT%
COPY %SYSTEM_32%\vcruntime140.dll			 	 			 %OPENTWIN_FRONTEND_DEPLOYMENT%
COPY %SYSTEM_32%\vcruntime140_1.dll			 				 %OPENTWIN_FRONTEND_DEPLOYMENT%

REM ==========================================
REM Copy ThirdParty files
REM ==========================================

ECHO.
ECHO ==========================================
ECHO 	Copy ThirdParty Libs
ECHO ==========================================

REM Curl
COPY "%CURL_DLLR%\libcurl.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM MongoDB
COPY "%MONGO_C_DLLR%\bson-1.0.dll"	 "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%MONGO_C_DLLR%\mongoc-1.0.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

COPY "%MONGO_CXX_DLLR%\bsoncxx.dll"	 "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%MONGO_CXX_DLLR%\mongocxx.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM OpenCASCADE
COPY "%TBB_DLLR%\tbb12.dll"  "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%TBB_DLLR%\tbbmalloc.dll"  "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%JEM_DLLR%\jemalloc.dll"   "%OPENTWIN_FRONTEND_DEPLOYMENT%"

COPY "%OC_DLLR%\TKBRep.dll" 	 "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OC_DLLR%\TKernel.dll" 	 "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OC_DLLR%\TKG2d.dll" 		 "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OC_DLLR%\TKG3d.dll" 		 "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OC_DLLR%\TKGeomAlgo.dll"  "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OC_DLLR%\TKGeomBase.dll"  "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OC_DLLR%\TKMath.dll" 	 "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OC_DLLR%\TKMesh.dll" 	 "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OC_DLLR%\TKShHealing.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OC_DLLR%\TKTopAlgo.dll" 	 "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM OpenSceneGraph-OpenSceneGraph-3.6.3
COPY "%OSG_DLLR%\OpenThreads.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

COPY "%OSG_DLLR%\osg.dll" 		"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OSG_DLLR%\osgDB.dll" 	"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OSG_DLLR%\osgGA.dll" 	"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OSG_DLLR%\osgText.dll" 	"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OSG_DLLR%\osgUtil.dll" 	"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OSG_DLLR%\osgViewer.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

MKDIR "%OPENTWIN_FRONTEND_DEPLOYMENT%\osgPlugins-3.6.3"
XCOPY /S "%OSG_DLLR%\osgPlugins-3.6.3" "%OPENTWIN_FRONTEND_DEPLOYMENT%\osgPlugins-3.6.3"

REM OpenSSL
COPY "%OPENSSL_DLL%\libcrypto-1_1-x64.dll"  "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OPENSSL_DLL%\libssl-1_1-x64.dll" 	"%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM Qt6
COPY "%QT_DLLR%\QT6Core.dll" 			"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%QT_DLLR%\QT6Gui.dll" 			"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%QT_DLLR%\QT6Network.dll" 		"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%QT_DLLR%\QT6OpenGL.dll" 			"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%QT_DLLR%\QT6OpenGLWidgets.dll" 	"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%QT_DLLR%\QT6Svg.dll" 			"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%QT_DLLR%\QT6WebSockets.dll" 		"%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%QT_DLLR%\QT6Widgets.dll" 		"%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM Qt-Advanced-Docking-System
COPY "%QT_ADS_ROOT%\lib\qtadvanceddocking.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM QT Plugins
MKDIR "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins"

MKDIR "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats"
COPY "%QT_PLUGINS%\imageformats\qgif.dll"  "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats"
COPY "%QT_PLUGINS%\imageformats\qicns.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats"
COPY "%QT_PLUGINS%\imageformats\qico.dll"  "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats"
COPY "%QT_PLUGINS%\imageformats\qjpeg.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats"
COPY "%QT_PLUGINS%\imageformats\qsvg.dll"  "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats"
COPY "%QT_PLUGINS%\imageformats\qtga.dll"  "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats"
COPY "%QT_PLUGINS%\imageformats\qtiff.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats"
COPY "%QT_PLUGINS%\imageformats\qwbmp.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats"
COPY "%QT_PLUGINS%\imageformats\qwebp.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats"

MKDIR "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\platforms"
COPY "%QT_PLUGINS%\platforms\qwindows.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\platforms"

MKDIR "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\tls"
COPY "%QT_PLUGINS%\tls\qcertonlybackend.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\tls"
COPY "%QT_PLUGINS%\tls\qopensslbackend.dll"  "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\tls"
COPY "%QT_PLUGINS%\tls\qschannelbackend.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\tls"

REM QtTabToolbar
COPY "%QT_TT_DLLR%\TabToolbar.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM Qwt
COPY "%QWT_LIB_DLLR%\qwt.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM ZLib
COPY "%ZLIB_DLLPATHR%\zlib.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM Python
MKDIR "%OPENTWIN_FRONTEND_DEPLOYMENT%\Python"

COPY "%OT_PYTHON_ROOT%\python.exe" "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OT_PYTHON_ROOT%\python39.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"
XCOPY /S "%OT_PYTHON_ROOT%\Lib" "%OPENTWIN_FRONTEND_DEPLOYMENT%\Python\Lib\" 
XCOPY /S "%OT_PYTHON_ROOT%\DLLs" "%OPENTWIN_FRONTEND_DEPLOYMENT%\Python\DLLs\" 

REM ==========================================
REM Copy OpenTwin Assets
REM ==========================================

ECHO.
ECHO ==========================================
ECHO 	Copy OpenTwin Assets
ECHO ==========================================

REM Icons
MKDIR "%OPENTWIN_FRONTEND_DEPLOYMENT%\icons"
XCOPY /S "%OPENTWIN_DEV_ROOT%\Assets\Icons" "%OPENTWIN_FRONTEND_DEPLOYMENT%\icons"

REM Colorstyles
MKDIR "%OPENTWIN_FRONTEND_DEPLOYMENT%\colorstyles"
XCOPY /S "%OPENTWIN_DEV_ROOT%\Assets\ColorStyles" "%OPENTWIN_FRONTEND_DEPLOYMENT%\colorstyles"

REM GraphicsItems
MKDIR "%OPENTWIN_FRONTEND_DEPLOYMENT%\GraphicsItems"
XCOPY /S "%OPENTWIN_DEV_ROOT%\Assets\GraphicsItems" "%OPENTWIN_FRONTEND_DEPLOYMENT%\GraphicsItems"

REM Fonts
MKDIR "%OPENTWIN_FRONTEND_DEPLOYMENT%\fonts"
XCOPY /S "%OPENTWIN_DEV_ROOT%\Assets\Fonts" "%OPENTWIN_FRONTEND_DEPLOYMENT%\fonts"

REM ==========================================
REM Copy OpenTwin build files
REM ==========================================

ECHO.
ECHO ==========================================
ECHO 	Copy OpenTwin libs
ECHO ==========================================

REM CADModelEntities
COPY "%OT_CADMODELENTITIES_ROOT%\%OT_DLLR%\CADModelEntities.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM DataStorage
COPY "%OT_DATASTORAGE_ROOT%\%OT_DLLR%\DataStorage.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM LTSpiceConnector
COPY "%OT_LTSPICE_CONNECTOR_ROOT%\%OT_DLLR%\LTSpiceConnector.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM ModelEntities
COPY "%OT_MODELENTITIES_ROOT%\%OT_DLLR%\ModelEntities.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM OTCommunication
COPY "%OT_COMMUNICATION_ROOT%\%OT_DLLR%\OTCommunication.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM OTCore
COPY "%OT_CORE_ROOT%\%OT_DLLR%\OTCore.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM OTGuiAPI
COPY "%OT_GUIAPI_ROOT%\%OT_DLLR%\OTGuiAPI.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM OTGui
COPY "%OT_GUI_ROOT%\%OT_DLLR%\OTGui.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM OTServiceFoundation
COPY "%OT_FOUNDATION_ROOT%\%OT_DLLR%\OTServiceFoundation.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM OTSystem
COPY "%OT_SYSTEM_ROOT%\%OT_DLLR%\OTSystem.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM OTWidgets
COPY "%OT_WIDGETS_ROOT%\%OT_DLLR%\OTWidgets.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM Rubberband
COPY "%OT_RUBBERBANDAPI_ROOT%\%OT_DLLR%\RubberbandEngineCore.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"
COPY "%OT_RUBBERBAND_ROOT%\%OT_DLLR%\RubberbandEngineOsgWrapper.dll"    "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM StudioSuiteConnector
COPY "%OT_STUDIO_SUITE_CONNECTOR_ROOT%\%OT_DLLR%\StudioSuiteConnector.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM UI Core
COPY "%OT_UICORE_ROOT%\%OT_DLLR%\uiCore.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM UI Service
COPY "%OT_UI_SERVICE_ROOT%\%OT_DLLR%\uiFrontend.exe" "%OPENTWIN_FRONTEND_DEPLOYMENT%"
REN "%OPENTWIN_FRONTEND_DEPLOYMENT%\uiFrontend.exe" OpenTwin.exe

REM Viewer
COPY "%OT_VIEWER_ROOT%\%OT_DLLR%\Viewer.dll" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM Certificates
MKDIR "%OPENTWIN_FRONTEND_DEPLOYMENT%\Certificates"
COPY "%OT_ENCRYPTIONKEY_ROOT%\ca.pem" "%OPENTWIN_FRONTEND_DEPLOYMENT%\Certificates"

ECHO [Paths] > "%OPENTWIN_FRONTEND_DEPLOYMENT%\qt.conf"
ECHO Plugins = .\\plugins >> "%OPENTWIN_FRONTEND_DEPLOYMENT%\qt.conf"

REM Copy the build information files
COPY "%OPENTWIN_DEV_ROOT%\Deployment\BuildInfo.txt" "%OPENTWIN_FRONTEND_DEPLOYMENT%"

REM ==========================================
REM Finally create the Installer
REM ==========================================

ECHO.
ECHO ==========================================
ECHO 	Creating Installer
ECHO ==========================================

CALL "C:\Program Files (x86)\NSIS\makensis.exe" Install-OpenTwin.nsi

REM move the installer into the Deployment folder for distribution via a server
MOVE "Install_OpenTwin_Frontend.exe" "%OPENTWIN_DEV_ROOT%\Deployment\FrontendInstaller"

exit /B 0

:PAUSE_END
pause
exit /B 1