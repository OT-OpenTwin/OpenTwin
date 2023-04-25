# To generate the root CA Cert-Key pair, run the following command that takes as a config file ca-csr.json, if you need to make changes, make them to this file

cfssl gencert -initca ca-csr.json | cfssljson -bare ca

# Generating a Server Cert-Key Pair through the following command

cfssl gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json -profile=server server-csr.json | cfssljson -bare server

This command uses the previously generated CA cert and key to sign a server's certificate. This certificate will have the properties defined in the server-csr.json file and it will use as a profile the server profile located in the ca-config.json.

For more information about the tool and more configuration properties, please check https://github.com/cloudflare/cfssl .

Please note: 
This directory contains already created ca.pem, ca-key.pem files for the certificates which are located in Microservices\SSL_certificates.
If a new server certificate should be created, these files should be reused to create compatible certificates.
FOR PRODUCTION USAGE, NEW CERTIFICATES NEED TO BE CREATED.