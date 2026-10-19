@ECHO OFF

"%~dp0python.exe" -B -m ot_launcher session %*
IF ERRORLEVEL 1 PAUSE
