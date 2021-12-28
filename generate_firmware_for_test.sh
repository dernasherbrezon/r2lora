#!/bin/bash

pio test -f testfirmware --without-testing --without-uploading

SUCCESS_JSON="./test/resources/fota/success.json"
INVALID_ZLIB_JZON="./test/resources/fota/invalidzlib.json"
MISMATCHED_CHECKSUM_JSON="./test/resources/fota/mismatchedchecksum.json"
VERSION="1.1"

echo "[" >> ${SUCCESS_JSON}
echo "[" >> ${INVALID_ZLIB_JZON}
echo "[" >> ${MISMATCHED_CHECKSUM_JSON}

find .pio/build -path "*/firmware.bin" -print0 | while read -d $'\0' file
do
    newname=$(echo ${file} | cut -d '/' -f 3)
    md5=($(md5sum ${file}))
    size=$(wc -c ${file} | cut -d ' ' -f 3)
    dstFilename=${newname}-${VERSION}.bin.zz
    pigz --zlib ${file}
    cp ${file}.zz ./test/resources/fota/${dstFilename}
    echo "{" >> ${SUCCESS_JSON}
    echo "\"name\": \"${newname}\"," >> ${SUCCESS_JSON}
    echo "\"version\": \"${VERSION}\"," >> ${SUCCESS_JSON}
    echo "\"filename\": \"/fota/${dstFilename}\"," >> ${SUCCESS_JSON}
    echo "\"size\": ${size}," >> ${SUCCESS_JSON}
    echo "\"md5Checksum\": \"${md5}\"" >> ${SUCCESS_JSON}
    echo "}," >> ${SUCCESS_JSON}

    invalidZlibFileName="invalidzlib-${VERSION}.bin.zz"
    invalidZlibFile="./test/resources/fota/${invalidZlibFileName}"
    echo "not a zlib" >> ${invalidZlibFile}
    size=$(wc -c ${invalidZlibFile} | cut -d ' ' -f 3)

    echo "{" >> ${INVALID_ZLIB_JZON}
    echo "\"name\": \"${newname}\"," >> ${INVALID_ZLIB_JZON}
    echo "\"version\": \"${VERSION}\"," >> ${INVALID_ZLIB_JZON}
    echo "\"filename\": \"/fota/${invalidZlibFileName}\"," >> ${INVALID_ZLIB_JZON}
    echo "\"size\": ${size}," >> ${INVALID_ZLIB_JZON}
    echo "\"md5Checksum\": \"${md5}\"" >> ${INVALID_ZLIB_JZON}
    echo "}," >> ${INVALID_ZLIB_JZON}

    echo "{" >> ${MISMATCHED_CHECKSUM_JSON}
    echo "\"name\": \"${newname}\"," >> ${MISMATCHED_CHECKSUM_JSON}
    echo "\"version\": \"${VERSION}\"," >> ${MISMATCHED_CHECKSUM_JSON}
    echo "\"filename\": \"/fota/${dstFilename}\"," >> ${MISMATCHED_CHECKSUM_JSON}
    echo "\"size\": ${size}," >> ${MISMATCHED_CHECKSUM_JSON}
    echo "\"md5Checksum\": \"6c0931332848636087c599a1ad9c0600\"" >> ${MISMATCHED_CHECKSUM_JSON}
    echo "}," >> ${MISMATCHED_CHECKSUM_JSON}

done

echo "]" >> ${SUCCESS_JSON}
echo "]" >> ${INVALID_ZLIB_JZON}
echo "]" >> ${MISMATCHED_CHECKSUM_JSON}