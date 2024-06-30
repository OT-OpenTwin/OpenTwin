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

REM Debug build
IF %DEBUG%==1 (
    ECHO %TYPE_NAME% DEBUG
    ECHO Running command: "%DEVENV_ROOT_2022%\devenv.exe" "%OT_SYSTEM_ROOT%\OTSystem.vcxproj" %TYPE% "Debug|x64" /Out buildLog_Debug.txt /Log devenv.log
    "%DEVENV_ROOT_2022%\devenv.exe" "%OT_SYSTEM_ROOT%\OTSystem.vcxproj" %TYPE% "Debug|x64" /Out buildLog_Debug.txt /Log devenv.log
    timeout /t 300 /nobreak
    IF EXIST buildLog_Debug.txt (
        ECHO Build log content:
        TYPE buildLog_Debug.txt
    ) ELSE (
        ECHO buildLog_Debug.txt does not exist or is empty.
    )
    IF EXIST devenv.log (
        ECHO Devenv log content:
        TYPE devenv.log
    ) ELSE (
        ECHO devenv.log does not exist or is empty.
    )
    IF ERRORLEVEL 1 (
        ECHO Debug build failed
        GOTO END
    )
    ECHO Debug build completed successfully
)

REM Release build
IF %RELEASE%==1 (
    ECHO %TYPE_NAME% RELEASE
    ECHO Running command: "%DEVENV_ROOT_2022%\devenv.exe" "%OT_SYSTEM_ROOT%\OTSystem.vcxproj" %TYPE% "Release|x64" /Out buildLog_Release.txt /Log devenv.log
    "%DEVENV_ROOT_2022%\devenv.exe" "%OT_SYSTEM_ROOT%\OTSystem.vcxproj" %TYPE% "Release|x64" /Out buildLog_Release.txt /Log devenv.log
    timeout /t 300 /nobreak
    IF EXIST buildLog_Release.txt (
        ECHO Build log content:
        TYPE buildLog_Release.txt
    ) ELSE (
        ECHO buildLog_Release.txt does not exist or is empty.
    )
    IF EXIST devenv.log (
        ECHO Devenv log content:
        TYPE devenv.log
    ) ELSE (
        ECHO devenv.log does not exist or is empty.
    )
    IF ERRORLEVEL 1 (
        ECHO Release build failed
        GOTO END
    )
    ECHO Release build completed successfully
)
  
GOTO END

:PAUSE_END
pause
GOTO END

:END
