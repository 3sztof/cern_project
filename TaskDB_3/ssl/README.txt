
Generate self signed SSL server certificate
===============================================

Note: need to accept security exception in forefox to access data!

See also:
https://www.akadia.com/services/ssh_test_certificate.html

## 1) Generate a Private Key
openssl genrsa -des3 -out server.key 1024
## 2) Generate a CSR (Certificate Signing Request)
openssl req -new -key server.key -out server.csr
##
## Country Name (2 letter code) [AU]:CH
## State or Province Name (full name) [Some-State]:Geneva
## Locality Name (eg, city) []:Geneva
## Organization Name (eg, company) [Internet Widgits Pty Ltd]:AAAA_LHCb_CERN
## Organizational Unit Name (eg, section) []:LHCb
## Common Name (e.g. server FQDN or YOUR name) []:localhost
## Email Address []:
##
## Please enter the following 'extra' attributes
## to be sent with your certificate request
## A challenge password []:
## An optional company name []:
##
## 3) Remove Passphrase from Key
cp server.key server.key.org
openssl rsa -in server.key.org -out server.key
## 4) Generating a Self-Signed Certificate
openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt
## 5) optional: generate pem file
cat server.crt server.key > server.pem
