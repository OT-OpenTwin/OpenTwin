@echo off

rem The first argument of the shell defines whether a release or debug build shall be performed. BOTH (default) , RELEASE, DEBUG 
rem The second argument of hte shell defines whether a full rebuild or just a build is performed. BUILD, REBUILD (default)
rem Please note that the commands are case-sensitive and that they must not be enclosed in quotes.

rem This script requires the following environment variables to be set:
rem 1. OPENTWIN_DEV_ROOT
rem 2. DEVENV_ROOT_2022

if "%OPENTWIN_DEV_ROOT%" == "" (
	echo Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

if "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	echo Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto PAUSE_END
)

if "%DEVENV_ROOT_2022%"=="" (
	echo Please specify the following environment variables: DEVENV_ROOT_2022
	goto END
)

if "%QtMsBuild%"=="" (
	echo Please install and set up the "Qt Visual Studio Tools" extension in Visual Studio, since it is required for build.
	goto END
)

rem ====================================================================
rem Preparations for the build process 
rem ====================================================================

rem Setup eviroment
call "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

rem Ensure that the script finished successfully
if not "%OPENTWIN_DEV_ENV_DEFINED%" == "1" (
	goto END
)

rem rem Enabled:
call "%OT_FILEHEADERUPDATER_ROOT%\x64\Release\FileHeaderUpdater.exe" --out "%OT_FILEHEADERUPDATER_ROOT%\FHU_Log.txt" --config "%OT_FILEHEADERUPDATER_ROOT%\OT_FHU_Config.json" %1 %2 %3 %4 %5 %6 %7 %8 %9

if "%1"=="--pause" (
	goto PAUSE_END
)
goto END

:PAUSE_END
pause
goto END

:END