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
ECHO Start the build and deploy batch script
ECHO ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"

DEL buildLog_Nightly.txt

CALL BatchBuildAndDeploy.bat > buildLog_Nightly.txt 2>&1

if %ERRORLEVEL% equ 0 (
	.\SendBuildEmailNotification.ps1 SUCCESSFUL
) else (
	.\SendBuildEmailNotification.ps1 FAILED
)
