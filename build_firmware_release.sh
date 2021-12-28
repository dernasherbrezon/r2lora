#!/bin/bash

pio run

mkdir -p target

FILE_INDEX="./target/r2lora.json"
VERSION="1.1"

rm -f ${FILE_INDEX}

echo "[" >> ${FILE_INDEX}

find .pio/build -path "*/firmware.bin" -print0 | while read -d $'\0' file
do
    newname=$(echo ${file} | cut -d '/' -f 3)
    md5=($(md5sum ${file}))
    dstFilename=${newname}-${VERSION}.bin.zz
    size=$(wc -c ${file} | awk '{print $1}')
    pigz --zlib ${file}
    cp ${file}.zz ./target/${dstFilename}
    echo "{" >> ${FILE_INDEX}
    echo "\"name\": \"${newname}\"," >> ${FILE_INDEX}
    echo "\"version\": \"${VERSION}\"," >> ${FILE_INDEX}
    echo "\"filename\": \"/fota/${dstFilename}\"," >> ${FILE_INDEX}
    echo "\"md5Checksum\": \"${md5}\"" >> ${FILE_INDEX}
    echo "}," >> ${FILE_INDEX}
    cp ${file} ${dstFilename}
done

echo "]" >> ${FILE_INDEX}