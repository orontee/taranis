#!/usr/bin/bash

PBI_FILENAME=taranis.pbi
PBI_PASSWORD=">nD4Ar0CNc(#Gl/87A8q"
FILES="_scriptInstall sinstall/taranis.app sinstall/icons/*"

cd ${MESON_BUILD_ROOT}/artifact
zip -e  --password "${PBI_PASSWORD}" ${PBI_FILENAME} ${FILES}
