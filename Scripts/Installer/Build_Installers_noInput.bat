@echo off
REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
setlocal enabledelayedexpansion

IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto END_FAIL
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto END_FAIL
)

REM First, we need to shutdown the application in case that services are still Running
CALL "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\ShutdownAll.bat"

IF "%OT_INSTALLIMAGES_DIR%" == "" (
    SET OT_INSTALLIMAGES_DIR="%OPENTWIN_DEV_ROOT%\InstallationImages"
)

RMDIR /S /Q "%OT_INSTALLIMAGES_DIR%"
MKDIR "%OT_INSTALLIMAGES_DIR%"

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
Set THIRDPARTY_ZIPFILE="!OPENTWIN_THIRDPARTY_ROOT!\Installer_Tools\ThirdParty.zip.001"
Set PLUGIN_ZIPFILE="!OPENTWIN_THIRDPARTY_ROOT!\Installer_Tools\ThirdParty\dev\EnVar_plugin.zip"

SET HELPER_PATH="!OPENTWIN_DEV_ROOT!\Scripts\Installer\helper"

if "!SEVENZIP_REG_DATA!"=="" (
	echo ERROR: 7Zip is not installed on your system!
	GOTO END_FAIL
)	else (
	echo 7Zip Installation verified in '!SEVENZIP_REG_DATA!'...
)

if "!NSIS_REG_VALUE!"=="" (
    echo NSIS Installation not found!
	GOTO END_FAIL
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
	"!SEVENZIP_REG_DATA!\7z.exe" x !THIRDPARTY_ZIPFILE! -o!THIRDPARTY_UNZIP_PATH! -y
	
REM	echo Extracting EnVar plugin for NSIS using 7-Zip...
REM	"!SEVENZIP_REG_DATA!\7z.exe" x !PLUGIN_ZIPFILE! -o!NSIS_REG_VALUE! -y -aos

	echo Copying Installation helpers...
	RMDIR /S /Q "!HELPER_PATH!"
	mkdir !HELPER_PATH!
	cd !HELPER_PATH!

	copy %OPENTWIN_DEV_ROOT%\Tools\SetPermissions\x64\Release\SetPermissions.exe .
	copy %OPENTWIN_DEV_ROOT%\Tools\ConfigMongoDBNoAuth\x64\Release\ConfigMongoDBNoAuth.exe .
	copy %OPENTWIN_DEV_ROOT%\Tools\ConfigMongoDBWithAuth\x64\Release\ConfigMongoDBWithAuth.exe .
	copy %OPENTWIN_DEV_ROOT%\Libraries\OTSystem\x64\Release\OTSystem.dll .
	cd %cd%

	echo COMPILING OPENTWIN INSTALLATION SCRIPTS
	!MAKENSIS_PATH! /V3 "/XOutFile "%OT_INSTALLIMAGES_DIR%\Install_OpenTwin.exe"" !ENDUSER_NSI!
	!MAKENSIS_PATH! /V3 "/XOutFile "%OT_INSTALLIMAGES_DIR%\Install_OpenTwin_Development.exe"" !DEVELOPER_NSI!
	GOTO END_SUCCESS

:END_SUCCESS
	echo ---------------------------------------------
	echo Script compilation has finished successfully. Exiting...
	GOTO EXIT
	
:END_FAIL
	echo ---------------------------------------------
	echo ERROR: The script has enountered an issue. Please try again.
	pause
	exit

:EXIT
endlocal
exit
