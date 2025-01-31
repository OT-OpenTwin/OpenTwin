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

REM Setup code documentation build path to developer documentation output.
if "%OPENTWIN_CODEDOC_BUILD_OUTPUT_ROOT%" == "" (
	set OPENTWIN_CODEDOC_BUILD_OUTPUT_ROOT=%OPENTWIN_DEV_ROOT%\Documentation\Developer\_build\html\_static
	goto PREPARE_MATHJAX_REL_PATH
)
goto START_BUILD

:PREPARE_MATHJAX_REL_PATH

REM This only works if the dev root and the third party root have the common parent directory
SET ot_lcl_batch_tmp=cd
cd %OPENTWIN_DEV_ROOT%
cd ..
SET ot_lcl_batch_tmp_dev=cd
cd %OPENTWIN_THIRDPARTY_ROOT%
cd ..
SET ot_lcl_batch_tmp_thi=cd
cd ot_lcl_batch_tmp

if "%ot_lcl_batch_tmp_dev%" == "%ot_lcl_batch_tmp_thi%" (
	goto SET_MATHJAX_REL_PATH
)

echo The operation can not be continued since the dev root and third party root don't have the same parent directory
goto PAUSE_END

:SET_MATHJAX_REL_PATH
for %%A in ("%OPENTWIN_THIRDPARTY_ROOT%") do set "ot_lcl_third_party_name=%%~nxA"
echo Directory name: %ot_lcl_third_party_name%
set MATHJAX_REL_PATH=..\..\..\..\..\..\%ot_lcl_third_party_name%\%MATHJAX_REL_PATH_SUFFIX%

:START_BUILD

ECHO ====================================================================
ECHO Build Sphinx Documentation
ECHO ====================================================================
PUSHD "%OT_DOCUMENTATION_ROOT%"
CALL "%OT_DOCUMENTATION_ROOT%\build.bat" > "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\Documentation_buildLog.txt"
POPD

RMDIR /S /Q "%OPENTWIN_DEV_ROOT%\Documentation\Developer\_build\html\_static\codedochtml"

ECHO ====================================================================
ECHO Build Doxygen Documentation
ECHO ====================================================================
CALL "%OPENTWIN_DEV_ROOT%\Documentation\Doxygen\Generate.bat" > "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\DoxygenDocumentation_buildLog.txt" 2>&1

RENAME "%OPENTWIN_DEV_ROOT%\Documentation\Developer\build\html\_static\codedochtml\index.xhtml" "code_doc_index.xhtml"

:END
