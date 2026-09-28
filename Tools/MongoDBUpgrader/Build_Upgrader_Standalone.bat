@ECHO OFF

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	GOTO END_FAIL
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	GOTO END_FAIL
)

CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\installers.py" upgrader
IF ERRORLEVEL 1 (
	PAUSE
	EXIT 0
)
EXIT

:END_FAIL
ECHO ---------------------------------------------
ECHO ERROR: The script has enountered an issue. Please try again.
PAUSE
EXIT
