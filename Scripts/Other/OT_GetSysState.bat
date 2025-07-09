set fileName=".\OT_Sys_State.log"

echo OPEN_TWIN_ADMIN_PORT: %OPEN_TWIN_ADMIN_PORT% > %fileName%
echo OPEN_TWIN_AUTH_PORT: %OPEN_TWIN_AUTH_PORT% >> %fileName%
echo OPEN_TWIN_CERTS_PATH: %OPEN_TWIN_CERTS_PATH% >> %fileName%
echo OPEN_TWIN_DOWNLOAD_PORT: %OPEN_TWIN_DOWNLOAD_PORT% >> %fileName%
echo OPEN_TWIN_GDS_PORT: %OPEN_TWIN_GDS_PORT% >> %fileName%
echo OPEN_TWIN_GSS_PORT: %OPEN_TWIN_GSS_PORT% >> %fileName%
echo OPEN_TWIN_LDS_PORT: %OPEN_TWIN_LDS_PORT% >> %fileName%
echo OPEN_TWIN_LSS_PORT: %OPEN_TWIN_LSS_PORT% >> %fileName%
echo OPEN_TWIN_MONGODB_ADDRESS: %OPEN_TWIN_MONGODB_ADDRESS% >> %fileName%
echo OPEN_TWIN_SERVICES_ADDRESS: %OPEN_TWIN_SERVICES_ADDRESS% >> %fileName%
echo CompSpec: %ComSpec% >> %fileName%

IF NOT "%OPEN_TWIN_CERTS_PATH%" == ""(
echo Cert folder content: >>%fileName%
dir "%OPEN_TWIN_CERTS_PATH%" >> %fileName%
)
REM sc qc "MongoDB" >> %fileName% 

setlocal enabledelayedexpansion

:: Get the BINARY_PATH_NAME line
for /f "tokens=2,* delims=:" %%A in ('sc qc MongoDB ^| findstr /i "BINARY_PATH_NAME"') do set LINE=%%B

:: Extract the config path
for /f tokens^=2^ delims^=^" %%A in ('echo %LINE% ^| findstr /i --config') do set CONFIG_PATH=%%A

:: Output result
echo MongoDB Config Path: !CONFIG_PATH! >>%fileName%
REM type input.txt >> %fileName%