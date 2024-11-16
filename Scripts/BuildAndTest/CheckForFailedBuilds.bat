@echo off
findstr /c:"Build failed" < buildLog_Summary.txt
if %errorlevel%==0 (
PowerShell -Command "Add-Type -AssemblyName PresentationFramework;[System.Windows.MessageBox]::Show('Project(s) have failed in build process')"
)

::pause