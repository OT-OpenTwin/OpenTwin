@ECHO OFF

"%~dp0python.exe" -B -m ot_launcher admin
IF ERRORLEVEL 1 PAUSE
