@echo off

REM The script allows an argument to be passed that will be passed trough to the set up LDS script.
REM Read the "OpenTwin_set_up_LDS.bat" script for more details.

REM ##########################################################################################

REM Service arguments

pushd "%~dp0" 
call OpenTwin_set_up_service_args.bat
popd

REM ##########################################################################################

pushd "%~dp0" 
call OpenTwin_set_up_LDS.bat %1
popd

if "%OT_LOCALDIRECTORYSERVICE_CONFIGURATION_DEFINED%" == "" (
	echo No LDS configuration created!
	goto PAUSE_END
)

goto END

:PAUSE_END
pause
goto END

:END
