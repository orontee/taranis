#!/usr/bin/bash

PROJECT_ROOT=${PWD}
# Script is expected to be called from project root

SDK_PARENT_DIR=${PROJECT_ROOT}/SDK_6.8.0
SDK_ROOT=${SDK_PARENT_DIR}/SDK-B288

cd ${SDK_ROOT}
./bin/update_path.sh

cd ${PROJECT_ROOT}
patch ${SDK_PARENT_DIR}/env_set.sh ${PROJECT_ROOT}/patchs/extend-env_set.sh.patch
${SDK_PARENT_DIR}/env_set.sh
# Will have to source that script later to set environment

PWDESC=$(echo ${SDK_PARENT_DIR} | sed 's_/_\\/_g')
sed "s/@pwd@/$PWDESC/g" crossfile_arm.ini.in > crossfile_arm.ini

find ${SDK_ROOT} -type f \
     \( -name "qt.conf" -or -name "Qt5*.cmake" \) \
     -execdir sed -i "s@/BUILD/@${PWDESC}/@g" {} \;

find ${SDK_ROOT}/usr/arm-obreey-linux-gnueabi/sysroot/usr/lib/pkgconfig \
     -type f -name "*.pc" \
     -execdir sed -i "s@/srv/Jenkins/workspace/SDK-GEN/output-b288/host@@g" {} \;

sed -i "s@Cflags:@#Cflags:@g" \
    ${SDK_ROOT}/usr/arm-obreey-linux-gnueabi/sysroot/usr/lib/pkgconfig/fontconfig.pc

PWDESC=$(echo ${PROJECT_ROOT} | sed 's_/_\\/_g')
sed "s/@pwd@/$PWDESC/g" crossfile_arm.ini.in > crossfile_arm.ini
