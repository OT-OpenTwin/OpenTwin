@ECHO OFF

REM It is expected that the session service and the authentification services
REM are already running on the remote computer

CALL OpenTwin_set_up_certificates.bat
CALL OpenTwin_set_up_services.bat

REM ===========================================================================
REM Launch the UI executable 
REM ===========================================================================

START /B uiFrontend.exe



