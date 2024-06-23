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

echo OPENTWIN_DEV_ROOT=%OPENTWIN_DEV_ROOT%
echo OPENTWIN_THIRDPARTY_ROOT=%OPENTWIN_THIRDPARTY_ROOT%
echo DEVENV_ROOT_2022=%DEVENV_ROOT_2022%

REM The first argument of the shell defines whether a release or debug build shall be performed. BOTH (default) , RELEASE, DEBUG 
REM The second argument of hte shell defines whether a full rebuild or just a build is performed. BUILD, REBUILD (default)
REM Please note that the commands are case-sensitive and that they must not be enclosed in quotes.

REM Call build script

echo "Starting RebuildAll.bat..."
call "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\RebuildAll.bat" BOTH BUILD
echo "Finished RebuildAll.bat"

echo "Starting CheckForFailedBuilds.bat..."
call "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\CheckForFailedBuilds.bat"
echo "Finished CheckForFailedBuilds.bat"


GOTO END

:PAUSE_END
pause
GOTO END

:END

REM Sets the exit code of the script to be 0
exit /B 0
