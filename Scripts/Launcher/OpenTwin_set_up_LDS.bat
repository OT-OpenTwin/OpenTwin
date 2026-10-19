@ECHO OFF

REM With the argument dev the services of %OPENTWIN_DEV_ROOT%\Deployment are used.

REM ot_launcher prints NAME=VALUE lines; SET makes them variables of the calling batch file.
FOR /F "delims=" %%V IN ('call "%~dp0python.exe" -B -m ot_launcher setup lds %1') DO SET "%%V"

IF NOT DEFINED OT_LOCALDIRECTORYSERVICE_CONFIGURATION_DEFINED PAUSE
