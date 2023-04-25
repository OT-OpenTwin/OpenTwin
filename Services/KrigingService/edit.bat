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

ECHO Setting up envi1ronment

REM Setup eviroment
CALL "%SIM_PLAT_ROOT%\MasterBuild\set_env.bat"

REM Setup Python dev env from third party libraries
CALL "%SIM_PLAT_ROOT%\Third_Party_Libraries\Python\set_paths_dev.bat"

ECHO Launching development enviroment

REM Open project
START "" "%DEVENV_ROOT_2022%\devenv.exe" "%SIM_PLAT_ROOT%\Libraries\KrigingService\KrigingService.vcxproj"

:END

