@ECHO OFF

REM The first argument of the shell defines whether a release or debug build shall be performed. BOTH (default) , RELEASE, DEBUG 
REM The second argument of hte shell defines whether a full rebuild or just a build is performed. BUILD, REBUILD (default)
REM Please note that the commands are case-sensitive and that they must not be enclosed in quotes.

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. DEVENV_ROOT_2022

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto PAUSE_END
)

REM ====================================================================
REM Preparations for the build process 
REM ====================================================================

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

REM Ensure that the script finished successfully
IF NOT "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)

ECHO ====================================================================
ECHO Build Doxygen Documentation
ECHO ====================================================================
CALL "%OPENTWIN_DEV_ROOT%\Documentation\Doxygen\Generate.bat" > "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\DoxygenDocumentation_buildLog.txt" 2>&1

ECHO ====================================================================
ECHO Copy Doxygen Documentation
ECHO ====================================================================
RMDIR /S /Q "%OPENTWIN_DEV_ROOT%\Documentation\Developer\_static\code"
XCOPY "%OPENTWIN_DEV_ROOT%\Documentation\Doxygen\html\*" "%OPENTWIN_DEV_ROOT%\Documentation\Developer\_static\code" /E /I /Y
RENAME "%OPENTWIN_DEV_ROOT%\Documentation\Developer\_static\code\index.html" "code_index.html"

ECHO ====================================================================
ECHO Build Sphinx Documentation
ECHO ====================================================================
PUSHD "%OT_DOCUMENTATION_ROOT%"
CALL "%OT_DOCUMENTATION_ROOT%\build.bat" > "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\Documentation_buildLog.txt"
POPD

:END
