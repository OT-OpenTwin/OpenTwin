@ECHO OFF

ECHO Setting up environment

rem Setup eviroment
CALL "%SIM_PLAT_ROOT%\MasterBuild\set_env.bat"

ECHO Testing Project : FITTDService

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
	"%DEVENV_ROOT_2022%\devenv.exe" "%SIM_PLAT_ROOT%\Libraries\FITTDService\FITTDService.vcxproj" %TYPE% "DebugTest|x64"
	ECHO %TYPE% DEBUG
	"%SIM_PLAT_ROOT%\Libraries\FITTDService\x64\Debug\FITTDServiceTest.exe" /Out --gtest_output="xml:%SIM_PLAT_ROOT%\MasterBuild\Reports\FITTDServiceDebugReport.xml"
	CALL "%SIM_PLAT_ROOT%\Third_Party_Libraries\Python\set_paths_dev.bat"
	python "%SIM_PLAT_ROOT%\MasterBuild\modifyXML.py" "%SIM_PLAT_ROOT%\MasterBuild\Reports\FITTDServiceDebugReport.xml" "FITTDService" "%SIM_PLAT_ROOT%\MasterBuild\EditReports\FITTDServiceDebugReport.xml"
)

IF %RELEASE%==1 (
	ECHO %TYPE% RELEASETEST
	"%DEVENV_ROOT_2022%\devenv.exe" "%SIM_PLAT_ROOT%\Libraries\FITTDService\FITTDService.vcxproj" %TYPE% "ReleaseTest|x64" 
	ECHO %TYPE% RELEASE
	"%SIM_PLAT_ROOT%\Libraries\FITTDService\x64\Release\FITTDServiceTest.exe" /Out --gtest_output="xml:%SIM_PLAT_ROOT%\MasterBuild\Reports\FITTDServiceReleaseReport.xml"
	CALL "%SIM_PLAT_ROOT%\Third_Party_Libraries\Python\set_paths_dev.bat"
	python "%SIM_PLAT_ROOT%\MasterBuild\modifyXML.py" "%SIM_PLAT_ROOT%\MasterBuild\Reports\FITTDServiceReleaseReport.xml" "FITTDService" "%SIM_PLAT_ROOT%\MasterBuild\EditReports\FITTDServiceReleaseReport.xml"

) 
  
:END










