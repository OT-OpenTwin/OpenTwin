@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 3. DEVENV_ROOT_2022
REM 4. OPENTWIN_FTP_PASSWORD

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

IF "%OPENTWIN_FTP_PASSWORD%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_FTP_PASSWORD
	goto PAUSE_END
)

ECHO ===================================================================
ECHO Get the latest version from the repository
ECHO ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%"
git pull
for /f "delims=" %%A in ('call git rev-parse --short HEAD') do set OT_VERSION=%%A

cd /D "%OPENTWIN_THIRDPARTY_ROOT%"
git pull
for /f "delims=" %%A in ('call git rev-parse --short HEAD') do set THIRD_PARTY_VERSION=%%A

set OT_VERSION_STRING=%OT_VERSION%:%THIRD_PARTY_VERSION%

IF "%OT_LAST_CONTIBUILD_VERSIONS%" == "%OT_VERSION_STRING%" (
	ECHO "The software is unchanged. No build necessary"
	exit /b 2
)

ECHO ===================================================================
ECHO Building Software for following commits:
ECHO OpenTwin = %OT_VERSION%
ECHO ThirdParty = %THIRD_PARTY_VERSION%
ECHO ===================================================================
ECHO ""

SETX OT_LAST_CONTIBUILD_VERSIONS %OT_VERSION_STRING%

ECHO ===================================================================
ECHO Build the software
ECHO ===================================================================

cd /D "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"
CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\RebuildAll.bat" BOTH BUILD

REM ===================================================================
REM Check the build results
REM ===================================================================

@echo off
findstr /c:"1 failed" < buildLog_Summary.txt
if %errorlevel%==0 (
ECHO ===================================================================
ECHO ERROR: Build failed
ECHO ===================================================================
exit /b 1
)
findstr /c:"2 failed" < buildLog_Summary.txt
if %errorlevel%==0 (
ECHO ===================================================================
ECHO ERROR: Build failed
ECHO ===================================================================
exit /b 1
)

exit /b 0
