@ECHO OFF

REM Shutdown microservices
taskkill /IM "open_twin.exe" /F 2> nul

taskkill /IM "PythonExecution.exe" /F 2> nul

REM Shutdown ui
taskkill /IM "uiFrontend.exe" /F 2> nul

REM Shutdown Apache server
taskkill /IM "httpd.exe" /F 2> nul

:waitloop
tasklist | find /I "httpd.exe" >nul
if not errorlevel 1 (
    timeout /t 1 >nul
    goto waitloop
)

:END
