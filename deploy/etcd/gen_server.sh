#!/usr/bin/env bash

# Generate valid Server Key/Cert
openssl genrsa -passout pass:1234 -des3 -out server.key 4096
openssl req -passin pass:1234 -new -key server.key -out server.csr -subj "/C=CN/ST=SH/L=SH/O=Test/OU=Server/CN=${1:-localhost}"
openssl x509 -req -passin pass:1234 -days 3650 -in server.csr -CA ca.crt -CAkey ca.key -set_serial 01 -out server.crt

# Remove passphrase from the Server Key
openssl rsa -passin pass:1234 -in server.key -out server.key

