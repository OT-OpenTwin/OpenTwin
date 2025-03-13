@echo off

REM The script allows an argument to be passed that will be passed trough to the set up LDS script.
REM Read the "OpenTwin_set_up_LDS.bat" script for more details.

REM ##########################################################################################

REM General

if not defined OPEN_TWIN_MONGODB_ADDRESS (
	set OPEN_TWIN_MONGODB_ADDRESS=127.0.0.1:27017
)

if not defined OPEN_TWIN_SERVICES_ADDRESS (
	set OPEN_TWIN_SERVICES_ADDRESS=127.0.0.1
)

if not defined OPEN_TWIN_TOOLKIT_PORT (
	set OPEN_TWIN_TOOLKIT_PORT=8001
)

if not defined OPEN_TWIN_LOG_PORT (
	set OPEN_TWIN_LOG_PORT=8090
)

if not defined OPEN_TWIN_GSS_PORT (
	set OPEN_TWIN_GSS_PORT=8091
)

if not defined OPEN_TWIN_LSS_PORT (
	set OPEN_TWIN_LSS_PORT=8093
)

if not defined OPEN_TWIN_AUTH_PORT (
	set OPEN_TWIN_AUTH_PORT=8092
)

if not defined OPEN_TWIN_GDS_PORT (
	set OPEN_TWIN_GDS_PORT=9094
)

if not defined OPEN_TWIN_LDS_PORT (
	set OPEN_TWIN_LDS_PORT=9095
)

if not defined OPEN_TWIN_ADMIN_PORT (
	set OPEN_TWIN_ADMIN_PORT=8000
)

if not defined OPEN_TWIN_SITE_ID  (
	set OPEN_TWIN_SITE_ID=1
)

if not defined OPEN_TWIN_DOWNLOAD_PORT  (
	set OPEN_TWIN_DOWNLOAD_PORT=80
)

REM ##########################################################################################

REM Logging url
if not defined OPEN_TWIN_LOGGING_URL (
	set OPEN_TWIN_LOGGING_URL=%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_LOG_PORT%
)

REM Log mode
if not defined OPEN_TWIN_LOGGING_MODE (
	REM Error and Warning
	set OPEN_TWIN_LOGGING_MODE="WARNING_LOG|ERROR_LOG"

	REM General (Info, Detailed, Warning, Error, NO message log)
	REM set OPEN_TWIN_LOGGING_MODE=ALL_GENERAL_LOG_FLAGS

	REM IO Messages (All inbound and outbound messages, NO general log)
	REM set OPEN_TWIN_LOGGING_MODE=ALL_MESSAGE_LOG_FLAGS

	REM All
	REM set OPEN_TWIN_LOGGING_MODE=ALL_LOG_FLAGS
)

pushd "%~dp0" 
call OpenTwin_set_up_LDS.bat %1
popd

if "%OT_LOCALDIRECTORYSERVICE_CONFIGURATION_DEFINED%" == "" (
	echo No LDS configuration created!
	goto PAUSE_END
)

goto END

:PAUSE_END
pause
goto END

:END
