#!/usr/bin/bash

ARCHIVE_FILENAME=taranis.zip

cd ${MESON_BUILD_ROOT}/artifact/sinstall
zip -r ../${ARCHIVE_FILENAME} .
