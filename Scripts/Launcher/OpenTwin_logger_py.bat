@ECHO OFF

"%~dp0python.exe" -B -m ot_launcher logger %*
IF ERRORLEVEL 1 PAUSE
