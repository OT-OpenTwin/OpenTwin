@ECHO OFF

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	PAUSE
	EXIT /B 0
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	PAUSE
	EXIT /B 0
)

IF "%DEVENV_ROOT_2022%" == "" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	PAUSE
	EXIT /B 0
)

CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\build_all.py" BOTH BUILD
"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\helpers.py" check-failed-builds

EXIT /B 0
