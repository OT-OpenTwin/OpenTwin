@ECHO OFF
REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 3. DEVENV_ROOT_2022

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

CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

ECHO Launching development enviroment

IF "%1" == "" (
	REM Open without project
	"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\run.py" --toolchain cmake-gui
)
ELSE (
	REM Open with project
	"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\run.py" --toolchain cmake-gui %1
)

GOTO END

REM In case we want the user to see the messages before closing the window
:PAUSE_END
pause
GOTO END

:END

