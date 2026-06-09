@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Setup Python dev env from third party Services
CALL "%OPENTWIN_THIRDPARTY_ROOT%\Python\set_paths_dev.bat"

REM Call the build shell
CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\BuildSingleProject.bat" "%OT_KRIGING_SERVICE_ROOT%\KrigingService.sln" %1 %2 

GOTO END

:PAUSE_END
pause
GOTO END

:END


