@ECHO OFF

REM The following environment variables can be defined to change the default settings:

REM OPEN_TWIN_MONGODB_ADDRESS  : Address of the MongoDB server, default: 127.0.0.1:27017 
REM OPEN_TWIN_SERVICES_ADDRESS : Address where the services shall be running, default: 127.0.0.1
REM OPEN_TWIN_LOG_PORT         : The port where the logger service shall be running, default: 8090
REM OPEN_TWIN_GSS_PORT         : The port where the global session service shall be running, default: 8091
REM OPEN_TWIN_AUTH_PORT        : The port where the authorisation service shall be running, default: 8092
REM OPEN_TWIN_LSS_PORT         : The port where the local session service shall be run, default: 8093
REM OPEN_TWIN_GDS_PORT         : The port where the global directory service shall be running, default: 9094
REM OPEN_TWIN_LDS_PORT         : The port where the local directory service shall be running, default: 9095
REM OPEN_TWIN_SITE_ID          : The ID of the current site, default: 1

CALL OpenTwin_set_up_certificates.bat
CALL OpenTwin_set_up_services.bat

REM VERBOSITY: Pass /V as argument to enable verbose mode
IF "%~1"=="/V" (
	REM OT is opening console windows in debug build, so we want to pause them at the end
	SET pause_prefix=cmd.exe /S /C "
	SET pause_suffix=" ^& pause
	ECHO ON
)

REM ===========================================================================
REM Launch the authorisation service 
REM ===========================================================================

REM Arguments of open_twin.exe:
REM 1) Name of the DLL containing the service logic
REM 2) -
REM 3) IP address of the service

START "OPEN TWIN TOOLKIT SERVICE" %pause_prefix%open_twin.exe OToolkit.dll "" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_TOOLKIT_PORT%" "" ""%pause_suffix%

GOTO END

:PAUSE_END
pause
GOTO END

:END
