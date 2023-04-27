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
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

ECHO Building Project

RMDIR /S /Q "%SIM_PLAT_ROOT%\Libraries\Model\.vs"
RMDIR /S /Q "%SIM_PLAT_ROOT%\Libraries\Model\x64"
RMDIR /S /Q "%SIM_PLAT_ROOT%\Libraries\Model\packages"

:END

