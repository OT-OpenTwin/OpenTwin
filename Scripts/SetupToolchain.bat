REM This script sets up the native C/C++ toolchain in the current shell.
REM The Ninja generator, unlike the Visual Studio generator, does not bootstrap the
REM compiler itself, so cl.exe, INCLUDE, LIB and LIBPATH have to be provided here.

REM This script requires the following environment variables to be set:
REM 1. DEVENV_ROOT_2022

REM Ensure that the setup will only be performed once
IF "%OT_TOOLCHAIN_READY%"=="1" (
	goto END
)

IF "%DEVENV_ROOT_2022%"=="" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto END
)

SET OT_LCL_VCVARS=%DEVENV_ROOT_2022%\..\..\VC\Auxiliary\Build\vcvars64.bat

IF NOT EXIST "%OT_LCL_VCVARS%" (
	ECHO Native toolchain not found: %OT_LCL_VCVARS%
	SET OT_LCL_VCVARS=
	goto END
)

CALL "%OT_LCL_VCVARS%" > nul 2>&1
SET OT_LCL_VCVARS=

IF "%INCLUDE%"=="" (
	ECHO Toolchain setup incomplete: vcvars64.bat did not provide INCLUDE
	goto END
)

IF "%LIB%"=="" (
	ECHO Toolchain setup incomplete: vcvars64.bat did not provide LIB
	goto END
)

REM Set the flag at the end to ensure everything else was completed successfully
SET OT_TOOLCHAIN_READY=1
ECHO OpenTwin native toolchain was set up successfully.

:END
