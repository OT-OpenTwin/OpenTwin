@echo on
findstr /c:"1 failed" < buildLog_Summary.txt
if %errorlevel%==0 (
  echo "Project(s) have failed in build process"
  exit /b 1
)
findstr /c:"2 failed" < buildLog_Summary.txt
if %errorlevel%==0 (
  echo "Project(s) have failed in build process"
  exit /b 1
)

::pause


