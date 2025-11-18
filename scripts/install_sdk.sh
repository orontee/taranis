#!/bin/bash

set -e

function show_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -f, --family (B288|B300)  Specify a device family"
    echo "  -p, --path PATH           Specify installation path"
    echo "  -h, --help                Show this help message"
}

DEVICE_FAMILY=""
SDK_PARENT_PATH=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        -f|--family)
            DEVICE_FAMILY="$2"
            shift 2
            ;;
        -p|--path)
            SDK_PARENT_PATH="$2"
            shift 2
            ;;
        -h|--help)
            show_help
	    exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

if [ -z "${DEVICE_FAMILY}" ]; then
    echo "Error: Missing device family"
    show_help
    exit 1
fi

if [ ! -d "${SDK_PARENT_PATH}" ]; then
    echo "Error: Installation path must exist"
    show_help
    exit 1
fi

SDK_ARCHIVE=SDK-${DEVICE_FAMILY}-6.8.7z

if [ "${DEVICE_FAMILY}" = "B288" ]; then
    SDK_NAME=SDK-B288
elif [ "${DEVICE_FAMILY}" = "B300" ]; then
    SDK_NAME=SDK-B300-6.8
else
    echo "Unexpected device family!"
    exit 1
fi
SDK_ROOT_PATH=${SDK_PARENT_PATH}/${SDK_NAME}

_7Z=$(which 7zz || which 7z)

function download_archive() {
    echo "Downloading ${SDK_ARCHIVE}"
    wget -nv -O "${SDK_ARCHIVE}" https://github.com/pocketbook/SDK_6.3.0/releases/download/6.8/"${SDK_ARCHIVE}" > /dev/null
    # 6.3.0 isn't a typo!
}

function unpack() {
    echo "Checking archive integrity"
    sha256sum -c "${SDK_ARCHIVE}.sha256"

    echo "Unpacking ${SDK_ARCHIVE}"

    # The archive contains dangerous symlinks. Some have relative
    # target path and can be created using 7z options. Others are
    # absolute and really dangerous, so they're skipped and equivalent
    # symlinks are created by hand, see fix_links.

    ${_7Z} x \
	-o${SDK_PARENT_PATH} \
	-snld \
	-xr\!${SDK_NAME}/usr/arm-obreey-linux-gnueabi/sysroot/etc/fonts/conf.d/ -- \
	"${SDK_ARCHIVE}"
}

function fix_links() {
    echo "Fixing links"

    local sysroot_path=${SDK_ROOT_PATH}/usr/arm-obreey-linux-gnueabi/sysroot
    local symlink_rootpath=${sysroot_path}/etc/fonts/conf.d
    local symlink_target_rootpath=../../../usr/share/fontconfig/conf.avail
    local links
    links="10-hinting-slight.conf 10-scale-bitmap-fonts.conf"
    links+=" 20-unhint-small-vera.conf 30-metric-aliases.conf"
    links+=" 30-urw-aliases.conf 40-nonlatin.conf 45-latin.conf"
    links+=" 49-sansserif.conf 50-user.conf 51-local.conf"
    links+=" 60-latin.conf 65-fonts-persian.conf 65-nonlatin.conf"
    links+=" 69-unifont.conf 80-delicious.conf 90-synthetic.conf"
    mkdir "${symlink_rootpath}"
    for link in ${links}; do
	ln -s "${symlink_target_rootpath}/${link}" "${symlink_rootpath}/${link}"
    done
}

function generate_env_script() {
    echo "Generation of env script"

    local env_script_path=${SDK_PARENT_PATH}/build_env.sh

    cat > "${env_script_path}" << EOF
DEVICE_FAMILY=${DEVICE_FAMILY}
SDK_ROOT_PATH=${SDK_ROOT_PATH}
PKG_CONFIG_PATH="${SDK_ROOT_PATH}/usr/arm-obreey-linux-gnueabi/sysroot/libcurl-nspr/usr/lib/pkgconfig/"
PKG_CONFIG_PATH+=":${SDK_ROOT_PATH}/usr/arm-obreey-linux-gnueabi/sysroot/usr/lib/pkgconfig"
PKG_CONFIG_PATH+=":${SDK_ROOT_PATH}/usr/arm-obreey-linux-gnueabi/sysroot/lib/pkgconfig"

export DEVICE_FAMILY
export SDK_ROOT_PATH
export PKG_CONFIG_PATH

echo "SDK 6.8 for ${DEVICE_FAMILY} devices is configured. Enjoy!"

EOF
    echo "Source ${SDK_PARENT_PATH}/build_env.sh to use SDK"
}

cd "${SDK_PARENT_PATH}"

if [ ! -d "${SDK_ROOT_PATH}" ]; then
    if [ ! -e "${SDK_ARCHIVE}" ]; then
	download_archive
    fi
    unpack
    fix_links
    ${SDK_ROOT_PATH}/bin/update_path.sh
    generate_env_script

fi
