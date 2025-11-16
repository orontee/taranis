#!/usr/bin/bash

# Expects current directory to be the build directory!

APPLICATION_FILENAME=taranis.app
INSTALLER_FILENAME=taranis.pbi
ARCHIVE_FILENAME=taranis.zip

SHA_FILE=taranis.sha256

if [[ ! -f "${APPLICATION_FILENAME}" ||
      ! -f "${INSTALLER_FILENAME}" ||
      ! -f "${ARCHIVE_FILENAME}" ]]; then
    echo "Error: Missing file"
fi

sha256sum ${APPLICATION_FILENAME} \
          ${INSTALLER_FILENAME} \
          ${ARCHIVE_FILENAME} > "${SHA_FILE}"
