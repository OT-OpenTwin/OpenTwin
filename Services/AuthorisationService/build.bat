@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. SIM_PLAT_ROOT
REM 2. DEVENV_ROOT_2022
IF "%SIM_PLAT_ROOT%" == "" (
	ECHO Please specify the following environment variables: SIM_PLAT_ROOT
	goto END
)

IF "%DEVENV_ROOT_2022%" == "" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto END
)

ECHO Setting up environment

rem Setup eviroment
CALL "%SIM_PLAT_ROOT%\MasterBuild\set_env.bat"

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
	"%DEVENV_ROOT_2022%\devenv.exe" "%SIM_PLAT_ROOT%\Libraries\AuthorisationService\AuthorisationService.vcxproj" %TYPE% "Debug|x64" /Out buildLog_Debug.txt
)

IF %RELEASE%==1 (
	ECHO %TYPE_NAME% RELEASE
	"%DEVENV_ROOT_2022%\devenv.exe" "%SIM_PLAT_ROOT%\Libraries\AuthorisationService\AuthorisationService.vcxproj" %TYPE% "Release|x64" /Out buildLog_Release.txt
) 
  
:END

