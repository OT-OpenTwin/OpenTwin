@ECHO OFF
REM Usage:
REM BuildSingleProject "project.sln" "RELEASE/DEBUG/BOTH" "BUILD/REBUILD" 
REM The environment variable OPENTWIN_DEV_PARALLEL_BUILDS defines how many parallel threads are allowed
REM for the compilation. If the variable is not set, the number of threads will be determined automatically

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 2. DEVENV_ROOT_2022
IF "%OPENTWIN_DEV_ROOT%"=="" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

IF "%OPENTWIN_THIRDPARTY_ROOT%"=="" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto PAUSE_END
)

IF "%DEVENV_ROOT_2022%"=="" (
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

IF "%2"=="RELEASE" (
  SET RELEASE=1
  SET DEBUG=0
)

IF "%2"=="DEBUG" (
  SET RELEASE=0
  SET DEBUG=1
)

REM Set the language to english
set VSLANG=1033

set ORIGINAL_DIR=%CD%

IF %DEBUG% neq 1 (
	GOTO RELEASE_TEST
)
	
ECHO %TYPE_NAME% DEBUG
ECHO ======================================================================================== >> testlog_Debug.txt
ECHO Testing project: %1 >> testlog_Debug.txt
ECHO ======================================================================================== >> testlog_Debug.txt
PUSHD "%1"
ctest -C Debug --test-dir build/windows-debug -V >> "%ORIGINAL_DIR%\testlog_Debug.txt"
POPD

:RELEASE_TEST

IF %RELEASE% neq 1 (
	GOTO END
)

ECHO %TYPE_NAME% RELEASE
ECHO ======================================================================================== >> testlog_Release.txt
ECHO Testing project: %1 >> testlog_Release.txt
ECHO ======================================================================================== >> testlog_Release.txt
PUSHD "%1"
ctest -C Release --test-dir build/windows-release -V >> "%ORIGINAL_DIR%\testlog_Release.txt"
POPD


  
GOTO END

:PAUSE_END
pause
GOTO END

:END