@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Call the build shell
CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\BuildSingleProject.bat" "%OT_PYTHON_EXECUTION_ROOT%\PythonExecution.sln" %1 %2 

GOTO END

:PAUSE_END
pause
GOTO END

:END

