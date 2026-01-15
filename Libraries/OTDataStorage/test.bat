@ECHO OFF

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

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)

SETLOCAL enabledelayedexpansion

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

SET "OLDPATH=%PATH%"

IF %DEBUG%==1 (
	SET "PATH=%OT_ALL_DLLD%;%ZLIB_DLLPATHD%;%OPENTWIN_DEV_ROOT%\Deployment;%OLDPATH%"
	CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\UnitTestSingleProject.bat" "%OT_DATASTORAGE_ROOT%" DEBUG
)

IF %RELEASE%==1 (
	SET "PATH=%OT_ALL_DLLR%;%OPENTWIN_DEV_ROOT%\Deployment;%OLDPATH%"
	CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\UnitTestSingleProject.bat" "%OT_DATASTORAGE_ROOT%" RELEASE
) 

SET "PATH=%OLDPATH%"

GOTO END

:PAUSE_END
pause
GOTO END

:END
