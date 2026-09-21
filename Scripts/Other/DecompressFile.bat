@ECHO OFF

REM This script will descompress all the files that were too large to push to git.
REM Note that the original file will NOT be deleted.
REM Note that the operation will be skipped when either the original file does not exist or the decompressed file exists

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto PAUSE_END
)

CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\helpers.py" decompress-file %1 %2

:END