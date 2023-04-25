@ECHO OFF

REM Shutdown microservices
taskkill /IM "open_twin.exe" /F

REM Shutdown ui
taskkill /IM "uiFrontend.exe" /F

REM Shutdown Apache server
taskkill /IM "httpd.exe" /F

:END