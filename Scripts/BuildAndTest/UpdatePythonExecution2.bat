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

REM ====================================================================
REM Setup eviroment, shutdown services and delete existing libraries
REM ====================================================================

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)

IF "%OPENTWIN_DEPLOYMENT_DIR%" == "" (
    SET OPENTWIN_DEPLOYMENT_DIR=%OPENTWIN_DEV_ROOT%\Deployment
)

CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\ShutdownAll.bat"

ECHO Delete libraries
DEL "%OPENTWIN_DEPLOYMENT_DIR%\PythonExecutionService.dll" 2>NUL
DEL "%OPENTWIN_DEPLOYMENT_DIR%\PythonExecution.exe" 2>NUL

ECHO Copy Services
COPY "%OT_PYTHON_EXECUTION_SERVICE_ROOT%2\%OT_DLLR%\PythonExecutionService.dll" "%OPENTWIN_DEPLOYMENT_DIR%"
COPY "%OT_PYTHON_EXECUTION_ROOT%2\%OT_DLLR%\PythonExecution.exe" "%OPENTWIN_DEPLOYMENT_DIR%"

GOTO END

:PAUSE_END
pause
GOTO END

:END
