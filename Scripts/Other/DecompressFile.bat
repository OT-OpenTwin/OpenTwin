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

REM Setup environment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
    echo Failed to set up Environment
	goto PAUSE_END
)

if "%1"=="" (
    echo Please specify a file when running this script
    goto END
)

if "%2"=="" (
    echo Please specify a output directory for the file
    goto END
)

set sourceFile=%1
set decompressedFolder=%2

REM Check source exists
if not exist %sourceFile% (
    echo Source file does not exist "%sourceFile%"
    goto END
)

REM Check destination does not exist
if not exist %decompressedFolder% (
    echo Destination folder does not exists "%decompressedFile%"
    goto END
)

REM Run expand
"%OPENTWIN_THIRDPARTY_ROOT%\7-Zip\Win64\7z.exe" x -o%decompressedFolder% %sourceFile%

:END