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

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)

ECHO Building Project

PUSHD "%OPENTWIN_DEV_ROOT%\Framework\OpenTwin"

REM Open project

SET RELEASE=1
SET DEBUG=1

IF "%1"=="RELEASE" (
  SET RELEASE=1
  SET DEBUG=0
)

IF "%1"=="DEBUG" (
  SET RELEASE=0
  SET DEBUG=1
)

SET REBUILD=1

IF "%2"=="BUILD" (
	SET REBUILD=0
)

IF %DEBUG%==1 (
	ECHO %TYPE_NAME% DEBUG
	
	DEL buildLog_Debug.txt
	
	if %REBUILD%==1 (
		cargo clean -p open_twin
	)

	cargo build 
		
	ECHO "Microservices OpenTwin Build RELEASE" > buildLog_Debug.txt
	FOR /F %%I IN ('where /r target\debug\.fingerprint output-lib-open_twin') DO TYPE %%I >> buildLog_Debug.txt
	FOR /F %%I IN ('where /r target\debug\.fingerprint output-bin-open_twin') DO TYPE %%I >> buildLog_Debug.txt

	REM Setup QT config
	DEL "%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\target\debug\qt.conf"
	COPY "%OPENTWIN_DEV_ROOT%\Assets\qt.conf" "%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\target\debug\qt.conf"
)

IF %RELEASE%==1 (
	ECHO %TYPE_NAME% RELEASE
	
	DEL buildLog_Release.txt

	if %REBUILD%==1 (
		cargo clean --release -p open_twin
	)

	cargo build --release 

	ECHO "Microservices OpenTwin Build DEBUG" > buildLog_Release.txt
	FOR /F %%I IN ('where /r target\release\.fingerprint output-lib-open_twin') DO TYPE %%I >> buildLog_Release.txt
	FOR /F %%I IN ('where /r target\release\.fingerprint output-bin-open_twin') DO TYPE %%I >> buildLog_Release.txt

	REM Setup QT config
	DEL "%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\target\release\qt.conf"
	COPY "%OPENTWIN_DEV_ROOT%\Assets\qt.conf" "%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\target\release\qt.conf"
) 

GOTO END

:PAUSE_END
pause
GOTO END

:END

POPD

TYPE "%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\buildLog_Debug.txt"
TYPE "%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\buildLog_Release.txt"
