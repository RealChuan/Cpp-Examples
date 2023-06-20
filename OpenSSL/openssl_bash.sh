#!/bin/bash

# OpenSSL Bash Script

# Generate a new private key and Certificate Signing Request
openssl req -out CSR.csr -new -newkey rsa:2048 -nodes -keyout privateKey.key

# Generate a self-signed certificate
openssl req -x509 -sha256 -nodes -days 365 -newkey rsa:2048 -keyout privateKey.key -out certificate.crt

# Generate a certificate signing request (CSR) for an existing private key
openssl req -out CSR.csr -key privateKey.key -new

# Generate a certificate signing request based on an existing certificate
openssl x509 -x509toreq -in certificate.crt -out CSR.csr -signkey privateKey.key

# Remove a passphrase from a private key
openssl rsa -in privateKey.pem -out newPrivateKey.pem

# RSA - Encrypt File
openssl rsautl -encrypt -inkey publicKey.pem -pubin -in plaintext.txt -out encrypted.txt

# RSA - Decrypt File
openssl rsautl -decrypt -inkey privateKey.pem -in encrypted.txt -out plaintext.txt

# RSA - Encrypt File with AES 256
openssl enc -aes-256-cbc -salt -in plaintext.txt -out encrypted.txt -pass file:./password.txt

# RSA - Decrypt File with AES 256
openssl enc -aes-256-cbc -d -in encrypted.txt -out decrypted.txt -pass file:./password.txt

# Generate a public key from a private key
openssl rsa -in privateKey.pem -pubout -out publicKey.pem

# Generate a CSR for a SAN (Subject Alternative Names) Certificate
openssl req -out CSR.csr -new -newkey rsa:2048 -nodes -keyout privateKey.key -config san.cnf

# Verify a Certificate Signing Request
openssl req -text -noout -verify -in CSR.csr

# Check a private key
openssl rsa -in privateKey.key -check

# Check a public key
openssl rsa -in publicKey.pem -pubin -check

# Check a certificate
openssl x509 -in certificate.crt -text -noout

# Check a PKCS#12 file (.pfx or .p12)
openssl pkcs12 -info -in keyStore.p12

# Convert a DER file (.crt .cer .der) to PEM
openssl x509 -inform der -in certificate.cer -out certificate.pem

# Convert a PEM file to DER
openssl x509 -outform der -in certificate.pem -out certificate.der

# Convert a PKCS#12 file (.pfx .p12) containing a private key and certificates to PEM
openssl pkcs12 -in keyStore.pfx -out keyStore.pem -nodes

# Convert a PEM certificate file and a private key to PKCS#12 (.pfx .p12)
openssl pkcs12 -export -out certificate.pfx -inkey privateKey.key -in certificate.crt -certfile CACert.crt

# Convert a PKCS#12 file (.pfx .p12) to PEM
openssl pkcs12 -in keyStore.pfx -out keyStore.pem -nodes

# Convert a PEM certificate file to DER
openssl x509 -outform der -in certificate.pem -out certificate.der

# Convert a PEM certificate file and a private key to PKCS#12 (.pfx .p12)
openssl pkcs12 -export -out certificate.pfx -inkey privateKey.key -in certificate.crt -certfile CACert.crt

# Convert a PKCS#12 file (.pfx .p12) containing a private key and certificates to PEM
openssl pkcs12 -in keyStore.pfx -out keyStore.pem -nodes

# Check a Certificate Signing Request (CSR)
openssl req -text -noout -verify -in CSR.csr
