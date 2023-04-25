@ECHO OFF

REM Check whether all certificate files are present

SET REQUIRE_CERTIFICATE_GENERATION=0
 
IF NOT EXIST "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca.pem" (
	SET REQUIRE_CERTIFICATE_GENERATION=1
)

IF NOT EXIST "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca-key.pem" (
	SET REQUIRE_CERTIFICATE_GENERATION=1
)

IF NOT EXIST "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server.pem" (
	SET REQUIRE_CERTIFICATE_GENERATION=1
)

IF NOT EXIST "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server-key.pem" (
	SET REQUIRE_CERTIFICATE_GENERATION=1
)

IF NOT EXIST "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\certificateKeyFile.pem" (
	SET REQUIRE_CERTIFICATE_GENERATION=1
)

REM Create the certificates if needed

IF %REQUIRE_CERTIFICATE_GENERATION%==1 (

	DEL "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca.pem"
	DEL "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\ca-key.pem"
	DEL "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server.pem"
	DEL "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\server-key.pem"
	DEL "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\certificateKeyFile.pem"

	cfssl gencert -initca ca-csr.json | cfssljson -bare ca

	cfssl gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json -profile=server server-csr.json | cfssljson -bare server
	
	TYPE server.pem server-key.pem > certificateKeyFile.pem

	COPY /Y ca.pem                 "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\"
	COPY /Y ca-key.pem             "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\"
	COPY /Y server.pem             "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\"
	COPY /Y server-key.pem         "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\"
	COPY /Y certificateKeyFile.pem "%SIM_PLAT_ROOT%\Microservices\SSL_certificates\"
	
	DEL ca.pem
	DEL ca-key.pem
	DEL server.pem
	DEL server-key.pem
	DEL ca.csr
	DEL server.csr
	DEL certificateKeyFile.pem

	ECHO SUCCESS: The certificates have been created successfully.
)


