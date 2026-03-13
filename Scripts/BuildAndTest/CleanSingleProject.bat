@ECHO OFF

rem This script requires the project path to be passed as argument 1
if "%1" == "" (
    echo Please specify the project path to use as argument 1 
	goto PAUSE_END
)

rem This script requires the following environment variables to be set:
rem 1. OPENTWIN_DEV_ROOT
rem 2. OPENTWIN_THIRDPARTY_ROOT
rem 3. DEVENV_ROOT_2022

if "%OPENTWIN_DEV_ROOT%" == "" (
	echo Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

if "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	echo Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto PAUSE_END
)

if "%DEVENV_ROOT_2022%" == "" (
	echo Please specify the following environment variables: DEVENV_ROOT_2022
	goto PAUSE_END
)

rem Setup eviroment
call "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

rem Ensure that the script finished successfully
if NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)

rmdir /S /Q "%1\.vs"
rmdir /S /Q "%1\build"
rmdir /S /Q "%1\x64"
rmdir /S /Q "%1\test"

GOTO END

:PAUSE_END
pause
GOTO END

:END
