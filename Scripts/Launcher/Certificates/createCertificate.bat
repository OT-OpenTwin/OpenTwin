@ECHO OFF

IF NOT DEFINED OPEN_TWIN_CERTS_PATH (
	ECHO ERROR: Please specify OPEN_TWIN_CERTS_PATH pointing to an existing directory
	GOTO END
)

CALL ..\OpenTwin_set_up_services.bat 

COPY /Y server-csr_template.json server-csr.json
fart server-csr.json $HOSTNAME$ %COMPUTERNAME%
fart server-csr.json $IP_ADDRESS$ %OPEN_TWIN_SERVICES_ADDRESS%

cfssl gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json -profile=server server-csr.json | cfssljson -bare server

TYPE server.pem server-key.pem > certificateKeyFile.pem

mkdir %OPEN_TWIN_CERTS_PATH%
COPY /Y ca.pem %OPEN_TWIN_CERTS_PATH%
COPY /Y server.pem %OPEN_TWIN_CERTS_PATH%
COPY /Y server-key.pem %OPEN_TWIN_CERTS_PATH%
COPY /Y certificateKeyFile.pem %OPEN_TWIN_CERTS_PATH%

ECHO SUCCESS: The certificates have been created successfully.

:END
ECHO.
PAUSE 0

