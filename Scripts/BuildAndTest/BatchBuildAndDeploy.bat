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

ECHO ===================================================================
ECHO Get the latest version from the repository
ECHO ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%"
git pull

cd /D "%OPENTWIN_THIRDPARTY_ROOT%"
git pull


ECHO ===================================================================
ECHO Build the software
ECHO ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"
CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\RebuildAll.bat" BOTH BUILD

REM ===================================================================
REM Check the build results
REM ===================================================================

@echo off
findstr /c:"Build failed" < buildLog_Summary.txt
if %errorlevel%==0 (
ECHO ===================================================================
ECHO ERROR: Build failed
ECHO ===================================================================
exit /b 1
)

ECHO ===================================================================
ECHO Build the documentation
ECHO ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"
CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\BuildDocumentation.bat"

ECHO ===================================================================
ECHO Create the deployment
ECHO ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"
CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\CreateDeployment.bat"

ECHO ===================================================================
ECHO Build the installers
ECHO ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\Installer"
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Installer\Build_Installers_noInput.bat"

ECHO ===================================================================
ECHO Upload the documentation and the nightly installers
ECHO ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"
"%OPENTWIN_THIRDPARTY_ROOT%\WinSCP\WinSCP.com" /ini=nul /script="%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\BatchBuildAndDeploy.txt"

exit /b 0
