@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. DEVENV_ROOT_2022
IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto END
)

IF "%DEVENV_ROOT_2022%" == "" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto END
)

ECHO Setting up environment

rem Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

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

IF %DEBUG%==1 (
	ECHO %TYPE_NAME% DEBUG
	"%DEVENV_ROOT_2022%\devenv.exe" "%OPENTWIN_DEV_ROOT%\Services\TetMeshService\TetMeshService.vcxproj" %TYPE% "Debug|x64" /Out buildLog_Debug.txt
)

IF %RELEASE%==1 (
	ECHO %TYPE_NAME% RELEASE
	"%DEVENV_ROOT_2022%\devenv.exe" "%OPENTWIN_DEV_ROOT%\Services\TetMeshService\TetMeshService.vcxproj" %TYPE% "Release|x64" /Out buildLog_Release.txt
) 
  
:END

