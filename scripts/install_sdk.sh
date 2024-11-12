#!/bin/bash

set -e

SDK_PATH=SDK_6.8.0
SDK_ARCHIVE=SDK-B288-6.8.7z

function download_archive() {
    echo Downloading ${SDK_ARCHIVE}
    wget -nv -O ${SDK_ARCHIVE} https://github.com/pocketbook/SDK_6.3.0/releases/download/6.8/${SDK_ARCHIVE} > /dev/null
}

function unpack() {
    sha256sum -c ${SDK_ARCHIVE}.sha256

    7z x -o${SDK_PATH} ${SDK_ARCHIVE}
}

if [ ! -d ${SDK_PATH} ]; then
    if [ ! -e ${SDK_ARCHIVE} ]; then
	download_archive
    fi
    unpack
fi
