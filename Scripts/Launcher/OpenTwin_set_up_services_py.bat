@ECHO OFF

CALL "%~dp0OpenTwin_set_up_service_args_py.bat"
CALL "%~dp0OpenTwin_set_up_LDS_py.bat" %1

IF NOT DEFINED OT_LOCALDIRECTORYSERVICE_CONFIGURATION_DEFINED (
	ECHO No LDS configuration created!
	PAUSE
)
