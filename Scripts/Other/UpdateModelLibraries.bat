@echo off
if not "%1"=="" set "OT_DEPLOYMENT_DIR=%1"
if not defined OT_DEPLOYMENT_DIR set "OT_DEPLOYMENT_DIR=%OPENTWIN_DEV_ROOT%\Deployment"
if not "%2"=="" set "DATABASE_PWD=%2"
if not defined DATABASE_PWD set "DATABASE_PWD="

echo [ModelLibraryUpdater] Deployment directory: %OT_DEPLOYMENT_DIR%
echo [ModelLibraryUpdater] Database password: %DATABASE_PWD%

if not "%OPENTWIN_LIBRARIES_UPDATE_REQUIRED%" == "1" (
    echo [ModelLibraryUpdater] No update required for model libraries
    exit /b 0
)

if not exist "%OT_DEPLOYMENT_DIR%\" (
    echo ERROR: Deployment directory not found: %OT_DEPLOYMENT_DIR%
    exit /b 1
)

pushd "%OT_DEPLOYMENT_DIR%"


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

SETX OPENTWIN_LIBRARIES_UPDATE_REQUIRED 0

exit /b 0