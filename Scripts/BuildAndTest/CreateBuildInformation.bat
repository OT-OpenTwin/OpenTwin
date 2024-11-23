@echo off
REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto END_FAIL
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto END_FAIL
)

set buildInfoFile="%OPENTWIN_DEV_ROOT%\Deployment\BuildInfo.txt"
del %buildInfoFile%

for /f "tokens=1-6 delims=.:-/ " %%a in ('echo %date% %time%') do (
    set day=%%a
    set month=%%b
    set year=%%c
    set currentTime=%%d:%%e:%%f
)

ECHO Build date/time: %year%-%month%-%day% %currentTime% >> %buildInfoFile%

cd %OPENTWIN_DEV_ROOT%
for /f %%i in ('git rev-parse --short HEAD') do set GIT_HASH=%%i
ECHO OpenTwin: %GIT_HASH% >> %buildInfoFile%

cd %OPENTWIN_THIRDPARTY_ROOT%
for /f %%i in ('git rev-parse --short HEAD') do set GIT_HASH=%%i
ECHO ThirdParty: %GIT_HASH% >> %buildInfoFile%


