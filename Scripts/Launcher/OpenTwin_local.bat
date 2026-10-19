@ECHO OFF

"%~dp0python.exe" -B -m ot_launcher local %*
IF ERRORLEVEL 1 PAUSE
