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

set buildInfoFile="%OPENTWIN_DEV_ROOT%\Scripts\Installer\helper\BuildInfo.txt"
del %buildInfoFile%

ECHO This installer was created with the following revisions: >> %buildInfoFile%

cd %OPENTWIN_DEV_ROOT%
ECHO OpenTwin Repo: >> %buildInfoFile%
call git rev-parse --short HEAD >>  %buildInfoFile%

cd %OPENTWIN_THIRDPARTY_ROOT%
ECHO Third Party Repo: >> %buildInfoFile%
call git rev-parse --short HEAD >> %buildInfoFile%


