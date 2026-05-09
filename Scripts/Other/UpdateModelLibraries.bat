@echo off
if not "%1"=="" set "OT_DEPLOYMENT_DIR=%1"
if not defined OT_DEPLOYMENT_DIR set "OT_DEPLOYMENT_DIR=%OPENTWIN_DEV_ROOT%\Deployment"
if not "%2"=="" set "DATABASE_PWD=%2"
if not defined DATABASE_PWD set "DATABASE_PWD="

if not exist "%OT_DEPLOYMENT_DIR%\" (
    echo ERROR: Deployment directory not found: %OT_DEPLOYMENT_DIR%
    exit /b 1
)

pushd "%OT_DEPLOYMENT_DIR%"
call "OpenTwin_logger.bat"
call "OpenTwin_session.bat" 

if not defined OPEN_TWIN_SERVICES_ADDRESS (
    echo ERROR: OPEN_TWIN_SERVICES_ADDRESS not set
    exit /b 1
)

if not defined OPEN_TWIN_LMS_PORT (
    echo ERROR: OPEN_TWIN_LMS_PORT not set
    exit /b 1
)

if not exist "%OPEN_TWIN_CA_CERT%" (
    echo ERROR: CA certificate file not found: %OPEN_TWIN_CA_CERT%
    exit /b 1
)

set "LMS_URL=%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_LMS_PORT%"

echo [ModelLibraryUpdater] LMS URL: %LMS_URL%

if not exist "%OT_DEPLOYMENT_DIR%\ModelLibraryUpdater.exe" (
    echo ERROR: ModelLibraryUpdater.exe not found
    exit /b 1
)

echo [1/2] Updating PythonEnvironments collection...
"%OT_DEPLOYMENT_DIR%\ModelLibraryUpdater.exe" --collection PythonEnvironments --lmsurl "%LMS_URL%" --dbpsw "%DATABASE_PWD%"

echo [2/2] Updating PythonScripts collection...
"%OT_DEPLOYMENT_DIR%\ModelLibraryUpdater.exe" --collection PythonScripts --lmsurl "%LMS_URL%" --dbpsw "%DATABASE_PWD%"    

:shutdown
if exist "%OT_DEPLOYMENT_DIR%\shutdownall.bat" call "%OT_DEPLOYMENT_DIR%\shutdownall.bat"
exit /b 0