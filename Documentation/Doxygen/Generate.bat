@echo off

rem This script requires the following environment variables to be set:
rem 1. OPENTWIN_DEV_ROOT
rem 2. OPENTWIN_THIRDPARTY_ROOT
rem 3. DEVENV_ROOT_2022
if "%OPENTWIN_DEV_ROOT%" == "" (
	echo Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

if "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	echo Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto PAUSE_END
)

rem ############################################################################################################################

rem Setup eviroment
call "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

rem Ensure that the script finished successfully
if not "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)

rem ############################################################################################################################

REM Setup code documentation build path to developer documentation output.
if "%OPENTWIN_CODEDOC_BUILD_OUTPUT_ROOT%" == "" (
	set "OPENTWIN_CODEDOC_BUILD_OUTPUT_ROOT=%OPENTWIN_DEV_ROOT%\Documentation\Developer\_build\html\_static"
)

if "%MATHJAX_REL_PATH%" == "" (
	goto GET_MATHJAX_RELATIVE_PATH
)

goto START_BUILD

:GET_MATHJAX_RELATIVE_PATH

REM This only works if the dev root and the third party root have the common parent directory
SET "ot_lcl_batch_tmp=cd"
cd %OPENTWIN_DEV_ROOT%
cd ..
SET "ot_lcl_batch_tmp_dev=cd"
cd %OPENTWIN_THIRDPARTY_ROOT%
cd ..
SET "ot_lcl_batch_tmp_thi=cd"
cd ot_lcl_batch_tmp

if "%ot_lcl_batch_tmp_dev%" == "%ot_lcl_batch_tmp_thi%" (
	goto SET_MATHJAX_REL_PATH
)

echo The operation can not be continued since the dev root and third party root don't have the same parent directory
goto PAUSE_END

:SET_MATHJAX_REL_PATH
for %%A in ("%OPENTWIN_THIRDPARTY_ROOT%") do set "ot_lcl_third_party_name=%%~nxA"
echo Directory name: %ot_lcl_third_party_name%
set "MATHJAX_REL_PATH=..\..\..\%ot_lcl_third_party_name%\%MATHJAX_REL_PATH_SUFFIX%"

if "%MATHJAX_REL_PATH%" == "" (
	echo Relative path not generated. Please provide MATHJAX_REL_PATH > "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\Documentation_buildLog.txt"
	goto PAUSE_END
)

rem ############################################################################################################################

:START_BUILD

rem Get the current timestamp (capture echo call from the batch)
for /f "delims=" %%a in ('call "%OPENTWIN_DEV_ROOT%\Scripts\Other\PrintCurrentTimestamp.bat" "Code doc generation finished at: " ""') do set OT_LCL_BuildStart=%%a

pushd "%OPENTWIN_DEV_ROOT%"

echo Building Doxygen documentation with following config:
echo - Output Path      = "%OPENTWIN_CODEDOC_BUILD_OUTPUT_ROOT%"
echo - MATHJAX_REL_PATH = "%MATHJAX_REL_PATH%"

doxygen "%OPENTWIN_DEV_ROOT%\Documentation\Doxygen\Doxyfile"

popd

rem Get the current timestamp (capture echo call from the batch)
 Get the current timestamp (capture echo call from the batch)
for /f "delims=" %%a in ('call "%OPENTWIN_DEV_ROOT%\Scripts\Other\PrintCurrentTimestamp.bat" "Code doc generation finished at: " ""') do set OT_LCL_BuildEnd=%%a

echo %OT_LCL_BuildStart%
echo %OT_LCL_BuildEnd%

goto END

:PAUSE_END

:END
