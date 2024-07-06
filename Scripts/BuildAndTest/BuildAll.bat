@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 3. DEVENV_ROOT_2022

ECHO Checking environment variables...
ECHO OPENTWIN_DEV_ROOT=%OPENTWIN_DEV_ROOT%
ECHO OPENTWIN_THIRDPARTY_ROOT=%OPENTWIN_THIRDPARTY_ROOT%
ECHO DEVENV_ROOT_2022=%DEVENV_ROOT_2022%

REM Check if required environment variables are set
IF "%OPENTWIN_DEV_ROOT%" == "" (
    ECHO ERROR: OPENTWIN_DEV_ROOT is not set.
    goto PAUSE_END
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
    ECHO ERROR: OPENTWIN_THIRDPARTY_ROOT is not set.
    goto PAUSE_END
)

IF "%DEVENV_ROOT_2022%" == "" (
    ECHO ERROR: DEVENV_ROOT_2022 is not set.
    goto PAUSE_END
)

ECHO Environment variables are set correctly.

REM The first argument of the shell defines whether a release or debug build shall be performed. BOTH (default), RELEASE, DEBUG
REM The second argument of the shell defines whether a full rebuild or just a build is performed. BUILD, REBUILD (default)
REM Please note that the commands are case-sensitive and must not be enclosed in quotes.

REM Call build script
ECHO Starting RebuildAll.bat...
call "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\RebuildAll.bat" BOTH BUILD
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: RebuildAll.bat failed with exit code %ERRORLEVEL%.
    goto END
)
ECHO Finished RebuildAll.bat

ECHO Starting CheckForFailedBuilds.bat...
call "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\CheckForFailedBuilds.bat"
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: CheckForFailedBuilds.bat failed with exit code %ERRORLEVEL%.
    goto END
)
ECHO Finished CheckForFailedBuilds.bat

GOTO END

:PAUSE_END
pause
GOTO END

:END
ECHO Script execution completed.
REM Sets the exit code of the script to be 0
exit /B 0
