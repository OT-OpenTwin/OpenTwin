@ECHO OFF

ECHO Setting up environment

rem Setup eviroment
CALL "%SIM_PLAT_ROOT%\MasterBuild\set_env.bat"

ECHO Building Project

PUSHD "%SIM_PLAT_ROOT%\Microservices\OpenTwin"

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
	DEL %SIM_PLAT_ROOT%\Microservices\OpenTwin\target\debug\qt.conf
	COPY "%SIM_PLAT_ROOT%\MasterBuild\qt.conf" "%SIM_PLAT_ROOT%\Microservices\OpenTwin\target\debug\qt.conf"
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
	DEL %SIM_PLAT_ROOT%\Microservices\OpenTwin\target\release\qt.conf
	COPY "%SIM_PLAT_ROOT%\MasterBuild\qt.conf" "%SIM_PLAT_ROOT%\Microservices\OpenTwin\target\release\qt.conf"
) 

:END

POPD

TYPE "%SIM_PLAT_ROOT%\Microservices\OpenTwin\buildLog_Debug.txt"
TYPE "%SIM_PLAT_ROOT%\Microservices\OpenTwin\buildLog_Release.txt"




