@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 3. DEVENV_ROOT_2022
REM 4. OPENTWIN_FTP_PASSWORD

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

IF "%OPENTWIN_FTP_PASSWORD%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_FTP_PASSWORD
	goto PAUSE_END
)

REM ===================================================================
REM Get the latest version from the repository
REM ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%"
git pull

cd /D "%OPENTWIN_THIRDPARTY_ROOT%"
git pull


REM ===================================================================
REM Call build script
REM ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"
CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\RebuildAll.bat" BOTH BUILD

REM ===================================================================
REM Check the build results
REM ===================================================================

@echo off
findstr /c:"1 failed" < buildLog_Summary.txt
if %errorlevel%==0 (
exit 1
)
findstr /c:"2 failed" < buildLog_Summary.txt
if %errorlevel%==0 (
exit 1
)

REM ===================================================================
REM Create the deployment
REM ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"
CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\CreateDeployment.bat"

REM ===================================================================
REM Build the installers
REM ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\Installer"
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Installer\Build_Installers_noInput.bat"

REM ===================================================================
REM Upload the documentation and the nightly installers
REM ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"
"%OPENTWIN_THIRDPARTY_ROOT%\WinSCP\WinSCP.com" /ini=nul /script="%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\BatchBuildAndDeploy.txt"

exit 0
