#!/bin/bash

set -e

SDK_PATH=SDK_6.8.0
SDK_ARCHIVE=SDK-B288-6.8.7z

_7Z=$(which 7zz || which 7z)

function download_archive() {
    echo Downloading ${SDK_ARCHIVE}
    wget -nv -O ${SDK_ARCHIVE} https://github.com/pocketbook/SDK_6.3.0/releases/download/6.8/${SDK_ARCHIVE} > /dev/null
}

function unpack() {
    sha256sum -c ${SDK_ARCHIVE}.sha256

    # The archive contains dangerous symlinks. Some have relative
    # target path and can be created using 7z options. Others are
    # absolute and really dangerous, so they're skipped and equivalent
    # symlinks are created by hand.

    ${_7Z} x \
	-o${SDK_PATH} \
	-snld \
	-xr\!SDK-B288/usr/arm-obreey-linux-gnueabi/sysroot/etc/fonts/conf.d/ -- \
	${SDK_ARCHIVE}

    local sysroot_path=${SDK_PATH}/SDK-B288/usr/arm-obreey-linux-gnueabi/sysroot
    local symlink_rootpath=${sysroot_path}/etc/fonts/conf.d
    local symlink_target_rootpath=../../../usr/share/fontconfig/conf.avail
    local links
    links="10-hinting-slight.conf 10-scale-bitmap-fonts.conf"
    links+=" 20-unhint-small-vera.conf 30-metric-aliases.conf"
    links+=" 30-urw-aliases.conf 40-nonlatin.conf 45-latin.conf"
    links+=" 49-sansserif.conf 50-user.conf 51-local.conf"
    links+=" 60-latin.conf 65-fonts-persian.conf 65-nonlatin.conf"
    links+=" 69-unifont.conf 80-delicious.conf 90-synthetic.conf"
    mkdir ${symlink_rootpath}
    for link in ${links}; do
	ln -s "${symlink_target_rootpath}/${link}" "${symlink_rootpath}/${link}"
    done
}

if [ ! -d ${SDK_PATH} ]; then
    if [ ! -e ${SDK_ARCHIVE} ]; then
	download_archive
    fi
    unpack
fi
