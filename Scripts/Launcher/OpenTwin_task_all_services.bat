@ECHO OFF
SETLOCAL

CD /D %~dp0

CALL OpenTwin_logger.bat %*
CALL OpenTwin_session.bat %*

