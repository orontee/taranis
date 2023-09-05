#!/usr/bin/bash

PROJECT_ROOT=${PWD}
# Script is expected to be called from project root

SDK_ROOT=${PROJECT_ROOT}/SDK_6.3.0/SDK-B288
cd ${SDK_ROOT}
./bin/update_path.sh

cd ${PROJECT_ROOT}
PWDESC=$(echo ${PROJECT_ROOT} | sed 's_/_\\/_g')
sed "s/@pwd@/$PWDESC/g" crossfile_arm.ini.in > crossfile_arm.ini
