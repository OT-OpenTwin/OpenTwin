@ECHO OFF

ECHO Setting up environment

rem Setup eviroment
CALL "%SIM_PLAT_ROOT%\MasterBuild\set_env.bat"

ECHO Testing Project : PHREECService

REM Open project

SET RELEASE=1
SET DEBUG=1

IF "%1"=="RELEASE" (
  SET RELEASE=1
  SET DEBUG=0
)

IF "%1"=="DEBUG" (
  SET RELEASE=0
  SET DEBUG=1
)

SET TYPE=/Rebuild
SET TYPE_NAME=REBUILD

IF "%2"=="BUILD" (
	SET TYPE=/Build
	SET TYPE_NAME=BUILD
)



IF %DEBUG%==1 (
	ECHO %TYPE% DEBUGTEST
	"%DEVENV_ROOT_2022%\devenv.exe" "%SIM_PLAT_ROOT%\Libraries\PHREECService\PHREECService.vcxproj" %TYPE% "DebugTest|x64"  
	ECHO %TYPE% DEBUG
	"%SIM_PLAT_ROOT%\Libraries\PHREECService\x64\Debug\PHREECServiceTest.exe" /Out --gtest_output="xml:%SIM_PLAT_ROOT%\MasterBuild\Reports\PHREECServiceDebugReport.xml"
	CALL "%SIM_PLAT_ROOT%\Third_Party_Libraries\Python\set_paths_dev.bat"
	python "%SIM_PLAT_ROOT%\MasterBuild\modifyXML.py" "%SIM_PLAT_ROOT%\MasterBuild\Reports\PHREECServiceDebugReport.xml" "PHREECService" "%SIM_PLAT_ROOT%\MasterBuild\EditReports\PHREECServiceDebugReport.xml"
)

IF %RELEASE%==1 (
	ECHO %TYPE% RELEASETEST
	"%DEVENV_ROOT_2022%\devenv.exe" "%SIM_PLAT_ROOT%\Libraries\PHREECService\PHREECService.vcxproj" %TYPE% "ReleaseTest|x64"
	ECHO %TYPE% RELEASE
	"%SIM_PLAT_ROOT%\Libraries\PHREECService\x64\Release\PHREECServiceTest.exe" /Out --gtest_output="xml:%SIM_PLAT_ROOT%\MasterBuild\Reports\PHREECServiceReleaseReport.xml"
	CALL "%SIM_PLAT_ROOT%\Third_Party_Libraries\Python\set_paths_dev.bat"
	python "%SIM_PLAT_ROOT%\MasterBuild\modifyXML.py" "%SIM_PLAT_ROOT%\MasterBuild\Reports\PHREECServiceReleaseReport.xml" "PHREECService" "%SIM_PLAT_ROOT%\MasterBuild\EditReports\PHREECServiceReleaseReport.xml"
) 
  
:END








