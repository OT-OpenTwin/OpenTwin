@ECHO OFF

SETLOCAL

CD /D %~dp0

"%~dp0python.exe" -B -m ot_launcher admin
