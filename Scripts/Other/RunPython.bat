@ECHO OFF

REM This script will compress the provided file
REM Note that the original file will NOT be deleted.
REM Note that the operation will be skipped when either the original file does not exist or the comressed file exists

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	exit(1);
    goto END
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	exit(1);
    goto END
)

CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

if "%1"=="" (
    echo Please specify a source file when running this script
    exit(2)
    goto END
)

REM Same as Python\set_paths_dev.bat: the legacy interpreter first on PATH
start "" "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\run.py" "PYTHONPATH=%%OT_PYTHONPATH_LEGACY%%" "PATH=%%OT_PYTHONPATH_LEGACY%%;%%PATH%%" python %1 %2 %3 %4 %5 %6 %7 %8 %9
goto END

:END
