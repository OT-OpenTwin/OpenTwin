@ECHO OFF

"%~dp0python.exe" -B -m ot_launcher toolkit %*
IF ERRORLEVEL 1 PAUSE
