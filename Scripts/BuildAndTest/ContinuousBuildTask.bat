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

IF "%OPEN_TWIN_EMAIL_LIST%" == "" (
	ECHO Please specify the following environment variables: OPEN_TWIN_EMAIL_LIST
	goto PAUSE_END
)

IF "%OPEN_TWIN_EMAIL_PWD%" == "" (
	ECHO Please specify the following environment variables: OPEN_TWIN_EMAIL_PWD
	goto PAUSE_END
)

ECHO ===================================================================
ECHO Start the build and deploy batch script
ECHO ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"

DEL buildLog_Continuous.txt

CALL ContinuousBuild.bat > buildLog_Continuous.txt 2>&1

if %ERRORLEVEL% equ 1 (
	powershell.exe -ExecutionPolicy Bypass -File "SendContiBuildEmailNotification.ps1" "FAILED"
) else (
	if %ERRORLEVEL% equ 2 (
		powershell.exe -ExecutionPolicy Bypass -File "SendContiBuildEmailNotification.ps1" "SUCCESSFUL"
	)
)
