@ECHO OFF

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

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REN %OPENTWIN_DEV_ROOT%\Documentation\Developer\_static _staticTMP
CALL "%OPENTWIN_DEV_ROOT%\Documentation\Developer\make.bat" html
REN %OPENTWIN_DEV_ROOT%\Documentation\Developer\_staticTMP _static

if "%1" == "silent" (
	goto END
)

:PAUSE_END
pause

:END
