@ECHO OFF

rem Setup eviroment
ECHO Setting up environment

CALL "%SIM_PLAT_ROOT%\MasterBuild\set_env.bat"

rem Build project
ECHO Building Project

yarn build



