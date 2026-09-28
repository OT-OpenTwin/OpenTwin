@ECHO OFF

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	EXIT /B 1
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	EXIT /B 1
)

CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\installers.py" build-information
