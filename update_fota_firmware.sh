#!/bin/bash

pio test -f testfirmware --without-testing --without-uploading

find .pio/build -path "*/firmware.bin" -print0 | while read -d $'\0' file
do
    newname=$(echo ${file} | cut -d '/' -f 3)
    mv ${file} ./test/resources/fota/${newname}-1.1.bin
done
