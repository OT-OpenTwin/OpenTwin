@ECHO OFF

REM This script will compress the provided file
REM Note that the original file will NOT be deleted.
REM Note that the operation will be skipped when either the original file does not exist or the comressed file exists

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

if "%1"=="" (
    echo Please specify a source file when running this script
    goto END
)

if "%2"=="" (
    echo Please specify a destination file when running this script
    goto END
)

set sourceFile=%1
set compressedFile=%2

REM Check source exists
if not exist %sourceFile% (
    echo Source file does not exist "%sourceFile%"
    goto END
)

REM Check destination does not exist
if exist %compressedFile% (
    echo Compressed file already exists "%compressedFile%"
    goto END
)

REM Run compress
"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\run.py" "%OPENTWIN_THIRDPARTY_ROOT%\7-Zip\Win64\7z.exe" a %compressedFile% %sourceFile%

:END