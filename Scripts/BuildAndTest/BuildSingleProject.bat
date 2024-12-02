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

ECHO Building Project %1

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

SET TYPE=/t:Rebuild
SET TYPE_NAME=REBUILD

IF "%3"=="BUILD" (
	SET TYPE=/t:Build
	SET TYPE_NAME=BUILD
)

SET PARALLEL_BUILD=""

IF "%OPENTWIN_DEV_PARALLEL_BUILDS%" NEQ "" (
	SET PARALLEL_BUILD=/p:CL_MPCount=%OPENTWIN_DEV_PARALLEL_BUILDS%
)

REM Set the language to english
set VSLANG=1033

IF %DEBUG% neq 1 (
	GOTO RELEASE_BUILD
)
	
ECHO %TYPE_NAME% DEBUG
ECHO ======================================================================================== >> buildlog_Debug.txt
ECHO Building project: %1 >> buildlog_Debug.txt
ECHO ======================================================================================== >> buildlog_Debug.txt
"%DEVENV_ROOT_2022%\..\..\MSBuild\Current\Bin\MSBuild.exe" "%1" %TYPE% %PARALLEL_BUILD% /Verbosity:minimal /p:Configuration=Debug /p:Platform=x64 >> buildlog_Debug.txt

if !ERRORLEVEL! neq 0 (
	IF "%3"=="BUILD" (
		ECHO ---------------------------------------------------------------------------------------- >> buildlog_Debug.txt
		ECHO Re-building project due to build failure: %1 >> buildlog_Debug.txt
		ECHO ---------------------------------------------------------------------------------------- >> buildlog_Debug.txt
		ver >nul
		"%DEVENV_ROOT_2022%\..\..\MSBuild\Current\Bin\MSBuild.exe" "%1" /t:Rebuild %PARALLEL_BUILD% /Verbosity:minimal /p:Configuration=Debug /p:Platform=x64 >> buildlog_Debug.txt
		
		if !ERRORLEVEL! neq 0 (
			ECHO --- Build failed: %1 --- >> buildlog_Debug.txt
		) ELSE (
			ECHO --- Build successful: %1 --- >> buildlog_Debug.txt
		)
	) ELSE (
		ECHO --- Build failed: %1 --- >> buildlog_Debug.txt	
	)
) ELSE (
	ECHO --- Build successful: %1 --- >> buildlog_Debug.txt
)

:RELEASE_BUILD

IF %RELEASE% neq 1 (
	GOTO END
)

ECHO %TYPE_NAME% RELEASE
ECHO ======================================================================================== >> buildlog_Release.txt
ECHO Building project: %1 >> buildlog_Release.txt
ECHO ======================================================================================== >> buildlog_Release.txt
"%DEVENV_ROOT_2022%\..\..\MSBuild\Current\Bin\MSBuild.exe" "%1" %TYPE% %PARALLEL_BUILD% /Verbosity:minimal /p:Configuration=Release /p:Platform=x64 >> buildlog_Release.txt

if !ERRORLEVEL! neq 0 (
	IF "%3"=="BUILD" (
		ECHO ---------------------------------------------------------------------------------------- >> buildlog_Release.txt
		ECHO Re-building project due to build failure: %1 >> buildlog_Release.txt
		ECHO ---------------------------------------------------------------------------------------- >> buildlog_Release.txt
		ver >nul
		"%DEVENV_ROOT_2022%\..\..\MSBuild\Current\Bin\MSBuild.exe" "%1" /t:Rebuild %PARALLEL_BUILD% /Verbosity:minimal /p:Configuration=Release /p:Platform=x64 >> buildlog_Release.txt
		
		if !ERRORLEVEL! neq 0 (
			ECHO --- Build failed: %1 --- >> buildlog_Release.txt
		) ELSE (
			ECHO --- Build successful: %1 --- >> buildlog_Release.txt
		)
	) ELSE (
		ECHO --- Build failed: %1 --- >> buildlog_Release.txt	
	)
) ELSE (
	ECHO --- Build successful: %1 --- >> buildlog_Release.txt
)
  
GOTO END

:PAUSE_END
pause
GOTO END

:END