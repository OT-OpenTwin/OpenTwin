@ECHO OFF

IF NOT DEFINED OPEN_TWIN_MONGODB_ADDRESS (
	SET OPEN_TWIN_MONGODB_ADDRESS=127.0.0.1:27017
)

IF NOT DEFINED OPEN_TWIN_SERVICES_ADDRESS (
	SET OPEN_TWIN_SERVICES_ADDRESS=127.0.0.1
)

IF NOT DEFINED OPEN_TWIN_TOOLKIT_PORT (
	SET OPEN_TWIN_TOOLKIT_PORT=8001
)

IF NOT DEFINED OPEN_TWIN_LOG_PORT (
	SET OPEN_TWIN_LOG_PORT=8090
)

IF NOT DEFINED OPEN_TWIN_GSS_PORT (
	SET OPEN_TWIN_GSS_PORT=8091
)

IF NOT DEFINED OPEN_TWIN_LSS_PORT (
	SET OPEN_TWIN_LSS_PORT=8093
)

IF NOT DEFINED OPEN_TWIN_AUTH_PORT (
	SET OPEN_TWIN_AUTH_PORT=8092
)

IF NOT DEFINED OPEN_TWIN_GDS_PORT (
	SET OPEN_TWIN_GDS_PORT=9094
)

IF NOT DEFINED OPEN_TWIN_LDS_PORT (
	SET OPEN_TWIN_LDS_PORT=9095
)

IF NOT DEFINED OPEN_TWIN_ADMIN_PORT (
	SET OPEN_TWIN_ADMIN_PORT=8000
)

IF NOT DEFINED OPEN_TWIN_SITE_ID  (
	SET OPEN_TWIN_SITE_ID=1
)

IF NOT DEFINED OPEN_TWIN_DOWNLOAD_PORT  (
	SET OPEN_TWIN_DOWNLOAD_PORT=80
)

IF NOT DEFINED OPEN_TWIN_LOGGING_URL (
	SET OPEN_TWIN_LOGGING_URL=%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_LOG_PORT%
)

IF NOT DEFINED OPEN_TWIN_LOGGING_MODE (
	REM Error and Warning log
	SET OPEN_TWIN_LOGGING_MODE="WARNING_LOG|ERROR_LOG"

	REM Log general (Info, Detailed, Warning, Error, NO message log)
	REM SET OPEN_TWIN_LOGGING_MODE=ALL_GENERAL_LOG_FLAGS

	REM Log Messages (All inbound and outbound messages, NO general log)
	REM SET OPEN_TWIN_LOGGING_MODE=ALL_MESSAGE_LOG_FLAGS

	REM Log All
	REM SET OPEN_TWIN_LOGGING_MODE=ALL_LOG_FLAGS
)

IF NOT DEFINED OT_LOCALDIRECTORYSERVICE_CONFIGURATION (
	CALL OpenTwin_set_up_LDS.bat
)

GOTO END

:PAUSE_END
pause
GOTO END

:END
