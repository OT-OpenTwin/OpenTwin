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

Set THIRDPARTY_UNZIP_PATH="!OPENTWIN_THIRDPARTY_ROOT!\Installer_Tools"
Set THIRDPARTY_ZIPFILE="!OPENTWIN_THIRDPARTY_ROOT!\Installer_Tools\ThirdParty.zip.001"

Set OT_INSTALLUPGRADER_DIR="%OPENTWIN_DEV_ROOT%\Tools\MongoDBUpgrader\Upgrader_Deployment"
RMDIR /S /Q "%OT_INSTALLUPGRADER_DIR%"
MKDIR "%OT_INSTALLUPGRADER_DIR%"

REM Getting the path for NSIS make
set NSIS_REG_KEY=HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\NSIS

set "NSIS_REG_VALUE="

for /f "tokens=2,*" %%a in ('reg query "%NSIS_REG_KEY%" /ve 2^>nul') do (
    set "NSIS_REG_VALUE=%%b"
)


Set MAKENSIS_PATH="!NSIS_REG_VALUE!\makensis.exe"

REM setting relevant paths
Set UPGRADER_NSI="!OPENTWIN_DEV_ROOT!\Tools\MongoDBUpgrader\Upgrader_NSIS\MongoDBUpgrader_Standalone.nsi"
Set UPGRADER_EXE="!OPENTWIN_DEV_ROOT!\Tools\MongoDBUpgrader\Upgrader_Exe\"

Set UPGRADER_Mongo_INSTALLER="!OPENTWIN_THIRDPARTY_ROOT!\Installer_Tools\ThirdParty\shared\MongoDB_Installer"
Set UPGRADER_SERVER="!OPENTWIN_THIRDPARTY_ROOT!\Installer_Tools\ThirdParty\shared\MongoDB_Server\"


MKDIR "%OT_INSTALLUPGRADER_DIR%\MongoDB_Server"
MKDIR "%OT_INSTALLUPGRADER_DIR%\MongoDB_Installer"
MKDIR "%OT_INSTALLUPGRADER_DIR%\Upgrader_Exe"


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
	"!OPENTWIN_THIRDPARTY_ROOT!\7-Zip\Win64\7z.exe" x !THIRDPARTY_ZIPFILE! -o!THIRDPARTY_UNZIP_PATH! -y

	REM First we build and copy the c++ executable
	CALL "%UPGRADER_EXE%build.bat" BOTH REBUILD
	copy %UPGRADER_EXE%\x64\Release\MongoDBUpgradeManager.exe %OT_INSTALLUPGRADER_DIR%\Upgrader_Exe\MongoDBUpgradeManager.exe
	
	REM Now copying all required dlls to the deployment dir
	copy %OPENTWIN_THIRDPARTY_ROOT%\boost\boost_1_86_0\lib64-msvc-14.3\boost_filesystem-vc143-mt-x64-1_86.dll %OT_INSTALLUPGRADER_DIR%\Upgrader_Exe\boost_filesystem-vc143-mt-x64-1_86.dll
	copy %OPENTWIN_THIRDPARTY_ROOT%\MongoDb\mongo-cxx-driver-r3.10.0\x64\Release\bin %OT_INSTALLUPGRADER_DIR%\Upgrader_Exe
	copy %OPENTWIN_THIRDPARTY_ROOT%\MongoDb\mongo-c-driver-1.27.3\x64\Release\bin %OT_INSTALLUPGRADER_DIR%\Upgrader_Exe

	REM Copying the required MongoDB files
	copy %UPGRADER_Mongo_INSTALLER%\mongodb-windows-x86_64-7.0.14-signed.msi %OT_INSTALLUPGRADER_DIR%\MongoDB_Installer\mongodb-windows-x86_64-7.0.14-signed.msi
	xcopy /e %UPGRADER_SERVER% %OT_INSTALLUPGRADER_DIR%\MongoDB_Server\
	
	echo COMPILING MONGODB UPGRADER SCRIPT
	!MAKENSIS_PATH! /V3 "/XOutFile "%OT_INSTALLUPGRADER_DIR%\UpgradeMongoDB.exe"" !UPGRADER_NSI!
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
