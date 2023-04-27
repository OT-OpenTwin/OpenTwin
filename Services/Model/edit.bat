@ECHO OFF

ECHO Setting up environment

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

ECHO Launching development enviroment

REM Open project
START "" "%DEVENV_ROOT_2022%\devenv.exe" "%OT_MODEL_SERVICE_ROOT%\Model.vcxproj"

:END

