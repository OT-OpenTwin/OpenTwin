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

ECHO Testing Project : OTRandom

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
	"%DEVENV_ROOT_2022%\devenv.exe" "%OT_RANDOM_ROOT%\OTRandom.vcxproj" %TYPE% "DebugTest|x64"  
	ECHO %TYPE% DEBUG
	"%OT_RANDOM_ROOT%\x64\Debug\OTRandomTest.exe" /Out --gtest_output="xml:%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\TestReports\OTRandomDebugReport.xml"
	CALL "%OPENTWIN_THIRDPARTY_ROOT%\Python\set_paths_dev.bat"
	python "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\modifyXML.py" "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\TestReports\OTRandomDebugReport.xml" "OTRandom" "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\EditReports\OTRandomDebugReport.xml"
)

IF %RELEASE%==1 (
	ECHO %TYPE% RELEASETEST
	"%DEVENV_ROOT_2022%\devenv.exe" "%OT_RANDOM_ROOT%\OTRandom.vcxproj" %TYPE% "ReleaseTest|x64"
	ECHO %TYPE% RELEASE
	"%OT_RANDOM_ROOT%\x64\Release\OTRandomTest.exe" /Out --gtest_output="xml:%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\TestReports\OTRandomReleaseReport.xml"
	CALL "%OPENTWIN_THIRDPARTY_ROOT%\Python\set_paths_dev.bat"
	python "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\modifyXML.py" "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\TestReports\OTRandomReleaseReport.xml" "OTRandom" "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\EditReports\OTRandomReleaseReport.xml"
) 
  
GOTO END

:PAUSE_END
pause
GOTO END

:END
