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

set NSIS_REG_KEY=HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\NSIS
set SEVENZIP_REG_KEY=HKEY_CURRENT_USER\SOFTWARE\7-Zip
set SEVENZIP_VALUE=Path
set "NSIS_REG_VALUE="

for /f "tokens=2,*" %%a in ('reg query "%NSIS_REG_KEY%" /ve 2^>nul') do (
    set "NSIS_REG_VALUE=%%b"
)

for /f "tokens=2*" %%a in ('reg query "%SEVENZIP_REG_KEY%" /v "%SEVENZIP_VALUE%" 2^>nul') do set SEVENZIP_REG_DATA=%%b


Set MAKENSIS_PATH="!NSIS_REG_VALUE!\makensis.exe"
Set ENDUSER_NSI="!OPENTWIN_DEV_ROOT!\Scripts\Installer\nsis\install_opentwin_endUser.nsi"
Set DEVELOPER_NSI="!OPENTWIN_DEV_ROOT!\Scripts\Installer\nsis\install_opentwin_Developer.nsi"

Set THIRDPARTY_UNZIP_PATH="!OPENTWIN_THIRDPARTY_ROOT!\Installer_Tools"
Set THIRDPARTY_ZIPFILE="!OPENTWIN_THIRDPARTY_ROOT!\Installer_Tools\ThirdParty.zip"

SET PYTHON_PATH="!OPENTWIN_DEV_ROOT!\Scripts\Installer\python"
Set PY_PERMISSIONS="!OPENTWIN_DEV_ROOT!\Scripts\Installer\python\change_permissions.py"
Set PY_MONGOD_NO_AUTH="!OPENTWIN_DEV_ROOT!\Scripts\Installer\python\mongoDB_storage_script_noAuth.py"
Set PY_MONGOD_AUTH="!OPENTWIN_DEV_ROOT!\Scripts\Installer\python\mongoDB_storage_script_wAuth.py"


REM Test for Python Installation
	python -V | find /v "Python" >NUL 2>NUL && (goto :PYTHON_NOT_INSTALLED)
	python -V | find "Python"    >NUL 2>NUL && (goto :PYTHON_INSTALLED)

:PYTHON_NOT_INSTALLED
	echo ERROR: Python is not installed on your system.
	pause
	GOTO EXIT

	
:PYTHON_INSTALLED
	for /f "delims=" %%V in ('python -V') do @set ver=%%V
	echo Python installation %ver% verified...


if "!NSIS_REG_VALUE!"=="" (
    echo NSIS Installation not found!
	pause
	GOTO EXIT
) else (
    echo NSIS Installation verified in '!NSIS_REG_VALUE!'...
	echo -------------------------------------------------------------
	echo Script compilation will take a few minutes, please be patient!
	echo Ready to compile!
	echo -------------------------------------------------------------
    GOTO COMPILE
)

:COMPILE
echo +++ COMPILE TIME +++

	echo Extracting Third Party Toolchain using 7-Zip...
	@REM echo !SEVENZIP_REG_DATA!
	@REM pause
	"!SEVENZIP_REG_DATA!\7z.exe" x !THIRDPARTY_ZIPFILE! -o!THIRDPARTY_UNZIP_PATH! -y
	@REM pause
	
	echo Compiling Python Scripts...
	cd !PYTHON_PATH!
	pyinstaller --onefile !PY_PERMISSIONS!
	pyinstaller --onefile !PY_MONGOD_NO_AUTH!
	pyinstaller --onefile !PY_MONGOD_AUTH!
	cd %cd%
	
	echo COMPILING OPENTWIN INSTALLATION SCRIPTS
	!MAKENSIS_PATH! /V3 !ENDUSER_NSI!
	!MAKENSIS_PATH! /V3 !DEVELOPER_NSI!
	GOTO END_SUCCESS

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
endlocal
exit
