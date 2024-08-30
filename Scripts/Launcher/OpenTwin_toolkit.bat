@ECHO OFF

REM The following environment variables can be defined to change the default settings:

REM OPEN_TWIN_SERVICES_ADDRESS : Address where the services shall be running, default: 127.0.0.1
REM OPEN_TWIN_TOOLKIT_PORT     : The port where the OToolkit shall be running, default: 8001

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
REM Launch the OToolkit
REM ===========================================================================

IF "%1"=="-logexport" (
	SET OPEN_TWIN_OTOOLKIT_ARGS=-logexport
)
IF "%2"=="-logexport" (
	SET OPEN_TWIN_OTOOLKIT_ARGS=-logexport
)

REM Arguments of open_twin.exe:
REM 1) Name of the DLL containing the service logic
REM 2) -
REM 3) IP address of the service

START "OPEN TWIN TOOLKIT SERVICE" %pause_prefix%open_twin.exe OToolkit.dll "" "%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_TOOLKIT_PORT%" "" "%OPEN_TWIN_OTOOLKIT_ARGS%"%pause_suffix%

GOTO END

:PAUSE_END
pause
GOTO END

:END
