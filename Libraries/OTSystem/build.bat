@ECHO ON

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 2. DEVENV_ROOT_2022
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

REM Echo environment variables for debugging
ECHO OPENTWIN_DEV_ROOT=%OPENTWIN_DEV_ROOT%
ECHO OPENTWIN_THIRDPARTY_ROOT=%OPENTWIN_THIRDPARTY_ROOT%
ECHO DEVENV_ROOT_2022=%DEVENV_ROOT_2022%

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
    ECHO Environment setup failed

	goto END
)

ECHO Building Project

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

ECHO Directory contents of devenv.exe path:
DIR "%DEVENV_ROOT_2022%"

ECHO Directory contents of OTSystem project path:
DIR "%OT_SYSTEM_ROOT%"

IF "%DEBUG%"=="1" (

    ECHO %TYPE_NAME% DEBUG
    DIR "%DEVENV_ROOT_2022%"
    ECHO Directory of devenv.exe: "%DEVENV_ROOT_2022%"
    DIR "%OT_SYSTEM_ROOT%"
    ECHO Directory of OTSystem root: %OT_SYSTEM_ROOT%
 
    "%DEVENV_ROOT_2022%\devenv.exe" "%OT_SYSTEM_ROOT%\OTSystem.vcxproj" %TYPE% "DEBUG|x64"
    IF ERRORLEVEL 1 (
        ECHO Error: Failed to build DEBUG configuration.
        EXIT /B 1
    )
)
IF "%RELEASE%"=="1" (
    ECHO %TYPE_NAME% RELEASE
    "%DEVENV_ROOT_2022%\devenv.exe" "%OT_SYSTEM_ROOT%\OTSystem.vcxproj" %TYPE% "Release|x64" 
    IF ERRORLEVEL 1 (
        ECHO Error: Failed to build RELEASE configuration.
        EXIT /B 1
    )
)

GOTO END

:PAUSE_END
pause
GOTO END

:END
