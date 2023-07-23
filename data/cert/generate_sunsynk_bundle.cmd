@echo off
REM Convert the Sunsynk PEM file to a binary blob
REM Uses gen_crt_bundle.py from ESP-IDF
REM Requires Python
python gen_crt_bundle.py --input cacrt_sunsynk.pem

REM Rename the binary file
DEL x509_crt_bundle.bin 2>NUL
REN x509_crt_bundle x509_crt_bundle.bin
