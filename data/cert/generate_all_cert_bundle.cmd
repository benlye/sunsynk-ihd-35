@echo off

REM Download CA bundle
REM Uses mk-ca-bundle.pl from Curl distribution. Downloads certdata.txt from Mozilla's source tree (see URL below).
REM Requires Perl.
perl .\mk-ca-bundle.pl -q -f -u

REM Append the DST Root CA X3 to the downloaded bundle
REM See dst_root_ca_x3.pem for information about why this is needed.
type .\ca-bundle.crt .\dst_root_ca_x3.pem > cert_bundle.pem

REM Convert the bundle to a binary blob
REM Uses gen_crt_bundle.py from ESP-IDF
REM Requires Python
python gen_crt_bundle.py --input cert_bundle.pem

REM Rename the binary file
DEL x509_crt_bundle.bin 2>NUL
REN x509_crt_bundle x509_crt_bundle.bin

REM Clean up
DEL ca-bundle.crt
