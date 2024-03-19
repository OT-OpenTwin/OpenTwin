@echo off
REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
setlocal enabledelayedexpansion

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto EXIT
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto EXIT
)

set KEY=HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\NSIS
set "REG_VALUE="

for /f "tokens=2,*" %%a in ('reg query "%KEY%" /ve 2^>nul') do (
    set "REG_VALUE=%%b"
)

Set MAKENSIS_PATH="!REG_VALUE!\makensis.exe"
Set ENDUSER_NSI="!OPENTWIN_DEV_ROOT!\Scripts\Installer\nsis\install_opentwin_endUser.nsi"
Set DEVELOPER_NSI="!OPENTWIN_DEV_ROOT!\Scripts\Installer\nsis\install_opentwin_Developer.nsi"

SET PYTHON_PATH="!OPENTWIN_DEV_ROOT!\Scripts\Installer\python"
Set PY_PERMISSIONS="!OPENTWIN_DEV_ROOT!\Scripts\Installer\python\change_permissions.py"
Set PY_MONGOD_NO_AUTH="!OPENTWIN_DEV_ROOT!\Scripts\Installer\python\mongoDB_storage_script_noAuth.py"
Set PY_MONGOD_AUTH="!OPENTWIN_DEV_ROOT!\Scripts\Installer\python\mongoDB_storage_script_wAuth.py"


REM Test for Python Installation
python -V | find /v "Python" >NUL 2>NUL && (goto :PYTHON_NOT_INSTALLED)
python -V | find "Python"    >NUL 2>NUL && (goto :PYTHON_INSTALLED)

:PYTHON_NOT_INSTALLED
	echo ERROR: Python is not installed on your system.
	set /p "Python_Input=Do you wish to install Python now? (y/n): "
	if /i "!Python_Input!"=="y" (
			GOTO INSTALL_PYTHON
		) else (
			GOTO END_FAIL
		)
GOTO EXIT

:INSTALL_PYTHON
	echo Installing Python...
	START "" /wait "!OPENTWIN_THIRDPARTY_ROOT!\Installer_Tools\Installer_Tools\ThirdParty\python-3.9.0-amd64.exe" /quiet InstallAllUsers=1 PrependPath=1 Include_test=0
	REM START "" /wait "!OPENTWIN_THIRDPARTY_ROOT!\Installer_Tools\Installer_Tools\cmd\RefreshEnv.cmd"

	if %ERRORLEVEL% EQU 0 (
		REM "!OPENTWIN_THIRDPARTY_ROOT!\Installer_Tools\Installer_Tools\cmd\RefreshEnv.cmd"
		REM pip -U pyinstaller
		echo Python Installed successfully! Please restart the batch process...
		GOTO EXIT
	) else (
		echo Python Installation failed! Aborting...
		GOTO EXIT
	)
	
	
:PYTHON_INSTALLED
for /f "delims=" %%V in ('python -V') do @set ver=%%V
echo Python installation %ver% verified...


if "!REG_VALUE!"=="" (
    echo NSIS Installation not found!
    set /p "NSIS_Install_Input=Do you wish to install NSIS now? (y/n): "
    if /i "!NSIS_Install_Input!"=="y" (
        GOTO INSTALL_NSIS
    ) else (
        GOTO END_FAIL
    )
) else (
    echo NSIS Installation verified in '!REG_VALUE!'...
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
	if /i "%ScriptInput%"=="4" (
	echo Cancelling compilation...
	GOTO EXIT
	)
	
	echo Compiling Python Scripts...
	cd !PYTHON_PATH!
	pyinstaller --onefile --noconsole !PY_PERMISSIONS!
	pyinstaller --onefile --noconsole !PY_MONGOD_NO_AUTH!
	pyinstaller --onefile --noconsole !PY_MONGOD_AUTH!
	cd %cd%
	
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

	Echo Invalid Input. Aborting...
	GOTO END_FAIL
		

GOTO END

:INSTALL_NSIS
echo Installing NSIS...
	START "" /wait "!OPENTWIN_THIRDPARTY_ROOT!\NullsoftScriptableInstallSystem\nsis-3.09-setup.exe"
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
