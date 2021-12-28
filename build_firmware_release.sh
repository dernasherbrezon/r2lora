#!/bin/bash

pio run

FILE_INDEX="r2lora.json"
VERSION="1.1"

rm -f ${FILE_INDEX}

echo "[" >> ${FILE_INDEX}

find .pio/build -path "*/firmware.bin" -print0 | while read -d $'\0' file
do
    newname=$(echo ${file} | cut -d '/' -f 3)
    md5=($(md5sum ${file}))
    dstFilename=${newname}-${VERSION}.bin
    echo "{" >> ${FILE_INDEX}
    echo "\"name\": \"${newname}\"," >> ${FILE_INDEX}
    echo "\"version\": \"${VERSION}\"," >> ${FILE_INDEX}
    echo "\"filename\": \"/fota/${dstFilename}\"," >> ${FILE_INDEX}
    echo "\"md5Checksum\": \"${md5}\"" >> ${FILE_INDEX}
    echo "}," >> ${FILE_INDEX}
    cp ${file} ${dstFilename}
done

echo "]" >> ${FILE_INDEX}