@ECHO OFF

REM ot_launcher prints NAME=VALUE lines; SET makes them variables of the calling batch file.
FOR /F "delims=" %%V IN ('call "%~dp0python.exe" -B -m ot_launcher setup service-args') DO SET "%%V"
