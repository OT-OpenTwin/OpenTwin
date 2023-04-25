@ECHO OFF

ECHO Setting up environment

REM Setup eviroment
CALL "%SIM_PLAT_ROOT%\MasterBuild\set_env.bat"

ECHO Launching development enviroment

REM Open project
START "" "%DEVENV_ROOT_2022%\devenv.exe" "%SIM_PLAT_ROOT%\Libraries\CADModelEntities\CADModelEntities.vcxproj"

:END

