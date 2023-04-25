@ECHO OFF

ECHO Setting up environment

rem Setup eviroment
CALL "%SIM_PLAT_ROOT%\MasterBuild\set_env.bat"

ECHO Testing Project : CADModelEntities

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
	"%DEVENV_ROOT_2022%\devenv.exe" "%SIM_PLAT_ROOT%\Libraries\CADModelEntities\CADModelEntities.vcxproj" %TYPE% "DebugTest|x64"  
	ECHO %TYPE% DEBUG
	"%SIM_PLAT_ROOT%\Libraries\CADModelEntities\x64\Debug\CADModelEntitiesTest.exe" /Out --gtest_output="xml:%SIM_PLAT_ROOT%\MasterBuild\Reports\CADModelEntitiesDebugReport.xml"
	CALL "%SIM_PLAT_ROOT%\Third_Party_Libraries\Python\set_paths_dev.bat"
	python "%SIM_PLAT_ROOT%\MasterBuild\modifyXML.py" "%SIM_PLAT_ROOT%\MasterBuild\Reports\CADModelEntitiesDebugReport.xml" "CADModelEntities" "%SIM_PLAT_ROOT%\MasterBuild\EditReports\CADModelEntitiesDebugReport.xml"
)

IF %RELEASE%==1 (
	ECHO %TYPE% RELEASETEST
	"%DEVENV_ROOT_2022%\devenv.exe" "%SIM_PLAT_ROOT%\Libraries\CADModelEntities\CADModelEntities.vcxproj" %TYPE% "ReleaseTest|x64"
	ECHO %TYPE% RELEASE
	"%SIM_PLAT_ROOT%\Libraries\CADModelEntities\x64\Release\CADModelEntitiesTest.exe" /Out --gtest_output="xml:%SIM_PLAT_ROOT%\MasterBuild\Reports\CADModelEntitiesReleaseReport.xml"
	CALL "%SIM_PLAT_ROOT%\Third_Party_Libraries\Python\set_paths_dev.bat"
	python "%SIM_PLAT_ROOT%\MasterBuild\modifyXML.py" "%SIM_PLAT_ROOT%\MasterBuild\Reports\CADModelEntitiesReleaseReport.xml" "CADModelEntities" "%SIM_PLAT_ROOT%\MasterBuild\EditReports\CADModelEntitiesReleaseReport.xml"
) 
  
:END















