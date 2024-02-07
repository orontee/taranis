#!/usr/bin/bash

APPLICATION_FILENAME=taranis.app
INSTALLER_FILENAME=taranis.pbi
ARCHIVE_FILENAME=taranis.zip
SHA_FILE=${MESON_BUILD_ROOT}/artifact/taranis.sha256

cd ${MESON_BUILD_ROOT}/artifact/sinstall
sha256sum ${APPLICATION_FILENAME} > ${SHA_FILE}

cd ${MESON_BUILD_ROOT}/artifact
sha256sum ${INSTALLER_FILENAME} >> ${SHA_FILE}
sha256sum ${ARCHIVE_FILENAME} >> ${SHA_FILE}
