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

cd /D "%SIM_PLAT_ROOT%\Microservices\Launcher"
CALL OpenTwin_set_up_certificates.bat
CALL OpenTwin_set_up_services.bat

cd /D "%SIM_PLAT_ROOT%\MasterBuild"

ECHO Shutdown: Local Directory Service
.\curl.exe --cert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server.pem" --key "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server-key.pem" --cacert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca.pem" -X POST -H "Content-Type: application/json" -d "{\"action\": \"exit\"}" -s https://%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_LDS_PORT%/shutdown

ECHO Shutdown: Global Directory Service
.\curl.exe --cert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server.pem" --key "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server-key.pem" --cacert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca.pem" -X POST -H "Content-Type: application/json" -d "{\"action\": \"exit\"}" -s https://%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_GDS_PORT%/shutdown

ECHO Shutdown: Local Session Service
.\curl.exe --cert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server.pem" --key "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server-key.pem" --cacert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca.pem" -X POST -H "Content-Type: application/json" -d "{\"action\": \"exit\"}" -s https://%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_LSS_PORT%/shutdown

ECHO Shutdown: Global Session Service
.\curl.exe --cert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server.pem" --key "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server-key.pem" --cacert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca.pem" -X POST -H "Content-Type: application/json" -d "{\"action\": \"exit\"}" -s https://%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_GSS_PORT%/shutdown

ECHO Shutdown: Authorisation Service
.\curl.exe --cert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server.pem" --key "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server-key.pem" --cacert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca.pem" -X POST -H "Content-Type: application/json" -d "{\"action\": \"exit\"}" -s https://%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_AUTH_PORT%/shutdown

ECHO Shutdown: Logging Service
REM .\curl.exe --cert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server.pem" --key "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server-key.pem" --cacert "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca.pem" -X POST -H "Content-Type: application/json" -d "{\"action\": \"exit\"}" -s https://%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_LOG_PORT%/shutdown

:END