#!/bin/bash

# Generate valid CA
openssl genrsa -passout pass:1234 -des3 -out ca.key 4096
openssl req -passin pass:1234 -new -x509 -days 3650 -key ca.key -out ca.crt -subj  "/C=CN/ST=SH/L=SH/O=Test/OU=Test/CN=Root CA"
