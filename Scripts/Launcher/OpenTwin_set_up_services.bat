@ECHO OFF

CALL "%~dp0OpenTwin_set_up_service_args.bat"
CALL "%~dp0OpenTwin_set_up_LDS.bat" %1

IF NOT DEFINED OT_LOCALDIRECTORYSERVICE_CONFIGURATION_DEFINED (
	ECHO No LDS configuration created!
	PAUSE
)
