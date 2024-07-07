@echo on
findstr /c:"1 failed" < buildLog_Summary.txt
if %errorlevel%==0 (
PowerShell -Command "Add-Type -AssemblyName PresentationFramework;[System.Windows.MessageBox]::Show('Project(s) have failed in build process')"
)
findstr /c:"2 failed" < buildLog_Summary.txt
if %errorlevel%==0 (
PowerShell -Command "Add-Type -AssemblyName PresentationFramework;[System.Windows.MessageBox]::Show('Project(s) have failed in build process')"
)

::pause
