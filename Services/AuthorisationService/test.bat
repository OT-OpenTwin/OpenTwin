@ECHO OFF

ECHO Setting up environment

rem Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

ECHO Testing Project : AuthorisationService

REM Open project

SET RELEASE=1
SET DEBUG=1

IF "%1"=="RELEASE" (
  SET RELEASE=1
  SET DEBUG=0
)

IF "%1"=="DEBUG" (
  SET RELEASE=0
  SET DEBUG=1
)

SET TYPE=/Rebuild
SET TYPE_NAME=REBUILD

IF "%2"=="BUILD" (
	SET TYPE=/Build
	SET TYPE_NAME=BUILD
)



IF %DEBUG%==1 (
	ECHO %TYPE% DEBUGTEST
	"%DEVENV_ROOT_2022%\devenv.exe" "%OT_AUTHORISATION_SERVICE_ROOT%\AuthorisationService.vcxproj" %TYPE% "DebugTest|x64" 
	ECHO %TYPE% DEBUG
	"%OT_AUTHORISATION_SERVICE_ROOT%\%OT_DLLD%\AuthorisationServiceTest.exe" /Out --gtest_output="xml:%OPENTWIN_DEV_ROOT%\Scripts\Reports\AuthorisationServiceDebugReport.xml"
	CALL "%OPENTWIN_THIRDPARTY_ROOT%\Python\set_paths_dev.bat"
	python "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\ModifyXML.py" "%OPENTWIN_DEV_ROOT%\Scripts\Reports\AuthorisationServiceDebugReport.xml" "AuthorisationService" "%OPENTWIN_DEV_ROOT%\Scripts\Reports\AuthorisationServiceDebugReport.xml"
)

IF %RELEASE%==1 (
	ECHO %TYPE% RELEASETEST
	"%DEVENV_ROOT_2022%\devenv.exe" "%OT_AUTHORISATION_SERVICE_ROOT%\AuthorisationService.vcxproj" %TYPE% "ReleaseTest|x64"
	ECHO %TYPE% RELEASE
	"%OT_AUTHORISATION_SERVICE_ROOT%\%OT_DLLR%\AuthorisationServiceTest.exe" /Out --gtest_output="xml:%OPENTWIN_DEV_ROOT%\Scripts\Reports\AuthorisationServiceReleaseReport.xml"
	CALL "%OPENTWIN_THIRDPARTY_ROOT%\Python\set_paths_dev.bat"
	python "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\ModifyXML.py" "%OPENTWIN_DEV_ROOT%\Scripts\Reports\AuthorisationServiceReleaseReport.xml" "AuthorisationService" "%OPENTWIN_DEV_ROOT%\Scripts\Reports\AuthorisationServiceReleaseReport.xml"
) 
  
:END


