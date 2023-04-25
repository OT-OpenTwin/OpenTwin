@ECHO OFF

REM The first argument of the shell defines whether a release or debug build shall be performed. BOTH (default) , RELEASE, DEBUG 
REM The second argument of hte shell defines whether a full rebuild or just a build is performed. BUILD, REBUILD (default)
REM Please note that the commands are case-sensitive and that they must not be enclosed in quotes.

REM Call build script

CALL "%SIM_PLAT_ROOT%\MasterBuild\rebuildAll.bat" BOTH BUILD

CALL "%SIM_PLAT_ROOT%\MasterBuild\CheckForFailedBuilds.bat"

REM Sets the exit code of the script to be 0
exit /B 0
