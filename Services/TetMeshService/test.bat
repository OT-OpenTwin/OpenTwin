@ECHO OFF

ECHO Setting up environment

rem Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

ECHO Testing Project : TetMeshService

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
	"%DEVENV_ROOT_2022%\devenv.exe" "%OPENTWIN_DEV_ROOT%\Services\TetMeshService\TetMeshService.vcxproj" %TYPE% "DebugTest|x64"  
	ECHO %TYPE% DEBUG
	"%OPENTWIN_DEV_ROOT%\Services\TetMeshService\x64\Debug\TetMeshServiceTest.exe" /Out --gtest_output="xml:%OPENTWIN_DEV_ROOT%\Scripts\Reports\TetMeshServiceDebugReport.xml"
	CALL "%OPENTWIN_THIRDPARTY_ROOT%\Python\set_paths_dev.bat"
	python "%OPENTWIN_DEV_ROOT%\Scripts\modifyXML.py" "%OPENTWIN_DEV_ROOT%\Scripts\Reports\TetMeshServiceDebugReport.xml" "TetMeshService" "%OPENTWIN_DEV_ROOT%\Scripts\EditReports\TetMeshServiceDebugReport.xml"
)

IF %RELEASE%==1 (
	ECHO %TYPE% RELEASETEST
	"%DEVENV_ROOT_2022%\devenv.exe" "%OPENTWIN_DEV_ROOT%\Services\TetMeshService\TetMeshService.vcxproj" %TYPE% "ReleaseTest|x64"
	ECHO %TYPE% RELEASE
	"%OPENTWIN_DEV_ROOT%\Services\TetMeshService\x64\Release\TetMeshServiceTest.exe" /Out --gtest_output="xml:%OPENTWIN_DEV_ROOT%\Scripts\Reports\TetMeshServiceReleaseReport.xml"
	CALL "%OPENTWIN_THIRDPARTY_ROOT%\Python\set_paths_dev.bat"
	python "%OPENTWIN_DEV_ROOT%\Scripts\modifyXML.py" "%OPENTWIN_DEV_ROOT%\Scripts\Reports\TetMeshServiceReleaseReport.xml" "TetMeshService" "%OPENTWIN_DEV_ROOT%\Scripts\EditReports\TetMeshServiceReleaseReport.xml"
) 
  
:END








