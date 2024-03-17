@echo off
setlocal enabledelayedexpansion

set KEY=HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\NSIS
set "REG_VALUE="

for /f "tokens=2,*" %%a in ('reg query "%KEY%" /ve 2^>nul') do (
    set "REG_VALUE=%%b"
)

Set MAKENSIS_PATH="!REG_VALUE!\makensis.exe"
Set ENDUSER_NSI=".\nsis\install_opentwin_endUser.nsi"
Set DEVELOPER_NSI=".\nsis\install_opentwin_Developer.nsi"

if "!REG_VALUE!"=="" (
    echo NSIS Installation not found!
    set /p "NSIS_Install_Input=Do you wish to install NSIS now? (y/n): "
    if /i "!NSIS_Install_Input!"=="y" (
        GOTO INSTALL_NSIS
    ) else (
        GOTO END_FAIL
    )
) else (
    echo NSIS Installation found in '!REG_VALUE!'
	echo -------------------------------------------------------------
	echo Script compilation will take a few minutes, please be patient!
	echo Ready to compile!
	echo -------------------------------------------------------------
    pause
    GOTO COMPILE
)

:COMPILE
echo +++ COMPILE TIME +++
	set /p ScriptInput=Which script would you like to compile? (1 = End-User, 2 = Developer, 3 = Both, 4 = Cancel) : 

	if /i "%ScriptInput%"=="1" (
		echo COMPILING OPENTWIN ENDUSER INSTALLATION SCRIPT
		!MAKENSIS_PATH! /V3 !ENDUSER_NSI!
		GOTO END_SUCCESS
	) 
	if /i "%ScriptInput%"=="2" (
		echo COMPILING OPENTWIN DEVELOPER INSTALLATION SCRIPT
		!MAKENSIS_PATH! /V3 !DEVELOPER_NSI!
		GOTO END_SUCCESS
	)
	if /i "%ScriptInput%"=="3" (
		echo COMPILING OPENTWIN INSTALLATION SCRIPTS
		!MAKENSIS_PATH! /V3 !ENDUSER_NSI!
		!MAKENSIS_PATH! /V3 !DEVELOPER_NSI!
		GOTO END_SUCCESS
	)
	if /i "%ScriptInput%"=="4" (
		echo Cancelling compilation...
		GOTO EXIT
	)
	Echo Invalid Input. Aborting...
	GOTO END_FAIL
		

GOTO END

:INSTALL_NSIS
echo Installing NSIS...
	START "" /wait "..\..\..\ThirdParty\NullsoftScriptableInstallSystem\nsis-3.09-setup.exe "
	if %ERRORLEVEL% EQU 0 (
		echo NSIS Installed successfully! Restarting batch process...
		START "" "%~0"
		exit
	) else (
		echo NSIS Installation failed! Aborting...
		GOTO EXIT
	)

:END_SUCCESS
	echo ---------------------------------------------
	echo Script compilation has finished. Exiting...
	GOTO EXIT
	
:END_FAIL
	echo ---------------------------------------------
	echo ERROR: The script has enountered an issue. Please try again.
	pause
	exit

:EXIT
pause
endlocal
exit
