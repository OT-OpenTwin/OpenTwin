@echo off
if "%1"=="" (
    echo ERROR: LibraryData directory argument missing
    exit /b 1
)

echo [ModelLibraryUpdater] LibraryData directory: %1
echo [ModelLibraryUpdater] Database password: %2

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

echo [1/2] Updating PythonEnvironments collection...
ModelLibraryUpdater.exe --data "%1\PythonEnvironments" --collection PythonEnvironments --lmsurl "%LMS_URL%" --dbpwd %2

echo [2/2] Updating PythonScripts collection...
ModelLibraryUpdater.exe --data "%1\PythonScripts" --collection PythonScripts --lmsurl "%LMS_URL%" --dbpwd %2    


exit /b 0