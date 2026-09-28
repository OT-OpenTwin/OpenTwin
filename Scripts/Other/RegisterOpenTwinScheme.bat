@ECHO OFF

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Registering OpenTwin URL protocol...
	ECHO ERROR: OPENTWIN_DEV_ROOT is not set.
	EXIT /B 1
)

CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\helpers.py" register-scheme
