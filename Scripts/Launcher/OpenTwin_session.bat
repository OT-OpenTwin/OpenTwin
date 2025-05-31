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
REM OPEN_TWIN_DOWNLOAD_PORT    : The port where the global session service will provide the download option for the frontend installer, default: 80

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
REM 2) IP address of this authorisation service
REM 3) IP address if MongoDB data base
REM 4) IP address of the logging service (if running)

START "AUTHORIZATION SERVICE" %pause_prefix%open_twin.exe AuthorisationService.dll "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_AUTH_PORT%" "%OPEN_TWIN_MONGODB_ADDRESS%" "%OPEN_TWIN_MONGODB_PWD%" "%OPEN_TWIN_LOGGING_URL%"%pause_suffix%

REM ===========================================================================
REM Launch the global session service 
REM ===========================================================================

REM Arguments of open_twin.exe:
REM 1) Name of the DLL containing the service logic
REM 2) IP address of the logging service (if running)
REM 3) IP address of this global session service
REM 4) IP address if MongoDB data base
REM 5) Port url of the authorisation service

START "GLOBAL SESSION SERVICE" %pause_prefix%open_twin.exe GlobalSessionService.dll "%OPEN_TWIN_LOGGING_URL%" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_GSS_PORT%" "%OPEN_TWIN_MONGODB_ADDRESS%" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_AUTH_PORT%" "%OPEN_TWIN_DOWNLOAD_PORT%"%pause_suffix%

REM ===========================================================================
REM Launch the session service 
REM ===========================================================================

REM Arguments of open_twin.exe:
REM 1) Name of the DLL containing the service logic
REM 2) IP address of the logging service (if running)
REM 3) IP address of this session service
REM 4) IP address if MongoDB data base
REM 5) Port number of the authorisation service

START "LOCAL SESSION SERVICE" %pause_prefix%open_twin.exe LocalSessionService.dll "%OPEN_TWIN_LOGGING_URL%" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_LSS_PORT%" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_GSS_PORT%" "%OPEN_TWIN_AUTH_PORT%"%pause_suffix%

REM ===========================================================================
REM Launch the global directory service 
REM ===========================================================================

REM Arguments of open_twin.exe:
REM 1) Name of the DLL containing the service logic
REM 2) IP address of the logging service (if running)
REM 3) IP address of this global directory service
REM 4) IP address of the global session service
REM 5) Unused

START "GLOBAL DIRECTORY SERVICE" %pause_prefix%open_twin.exe GlobalDirectoryService.dll "%OPEN_TWIN_LOGGING_URL%" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_GDS_PORT%" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_GSS_PORT%" "unused"%pause_suffix%

REM ===========================================================================
REM Launch the local directory service 
REM ===========================================================================

REM Arguments of open_twin.exe:
REM 1) Name of the DLL containing the service logic
REM 2) IP address of the logging service (if running)
REM 3) IP address of this local directory service
REM 4) IP address of the global directory service
REM 5) Unused

START "LOCAL DIRECTORY SERVICE" %pause_prefix%open_twin.exe LocalDirectoryService.dll "%OPEN_TWIN_LOGGING_URL%" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_LDS_PORT%" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_GDS_PORT%" "%OPEN_TWIN_AUTH_PORT%"%pause_suffix%

REM ===========================================================================
REM Launch the local directory service 
REM ===========================================================================

REM Arguments of open_twin.exe:
REM 1) Name of the DLL containing the service logic
REM 2) IP address of the logging service (if running)
REM 3) IP address of this local directory service
REM 4) IP address of the global directory service
REM 5) Unused

START "LIBRARY MANAGEMENT SERVICE" %pause_prefix%open_twin.exe LibraryManagementService.dll "%OPEN_TWIN_LOGGING_URL%" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_LMS_PORT%" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_GSS_PORT%" "%OPEN_TWIN_MONGODB_PWD%" %pause_suffix%

REM ===========================================================================
REM Launch the admin panel web server
REM ===========================================================================

CALL OpenTwin_admin.bat
