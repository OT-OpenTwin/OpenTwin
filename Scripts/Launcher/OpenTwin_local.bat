@ECHO OFF
SETLOCAL

CALL OpenTwin_logger.bat %*
CALL OpenTwin_session.bat %*

REM ===========================================================================
REM Launch the UI executable 
REM ===========================================================================

START /B uiFrontend.exe
