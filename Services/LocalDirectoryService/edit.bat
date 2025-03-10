@echo off

:: This script requires the following environment variables to be set:
:: 1. OPENTWIN_DEV_ROOT
:: 2. OPENTWIN_THIRDPARTY_ROOT
:: 3. DEVENV_ROOT_2022

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

:: Setup eviroment
call "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

:: Ensure that the script finished successfully
if not "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\Launcher"

call OpenTwin_set_up_services.bat dev

echo Launching development enviroment

:: Open project
start "" "%DEVENV_ROOT_2022%\devenv.exe" "%OT_LOCAL_DIRECTORY_SERVICE_ROOT%\LocalDirectoryService.vcxproj"

goto END

:PAUSE_END
pause
goto END

:END
