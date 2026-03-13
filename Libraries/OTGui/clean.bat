@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

call "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\CleanSingleProject.bat %OT_GUI_ROOT%"

GOTO END

:PAUSE_END
pause
GOTO END

:END

