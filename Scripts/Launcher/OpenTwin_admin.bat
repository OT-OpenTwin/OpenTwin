@ECHO OFF
SETLOCAL

CALL OpenTwin_set_up_services.bat

REM ===========================================================================
REM Launch the UI executable 
REM ===========================================================================

SET SRVROOT=%cd%/Apache

PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& './OpenTwin_admin.ps1'"





