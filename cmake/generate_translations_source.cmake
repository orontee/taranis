# Generate source with translations

find_program(GENERATE_L10N_CC
  NAMES
  generate_l10n_cc.py
  PATHS
  scripts
  REQUIRED
  DOC "Generate C++ files from PO files"
)

add_custom_command(
  OUTPUT l10n.cc
  COMMAND ${GENERATE_L10N_CC} --output ${CMAKE_CURRENT_BINARY_DIR}/l10n.cc --template ${CMAKE_SOURCE_DIR}/src/l10n.cc.in ${CMAKE_SOURCE_DIR}/po
  DEPENDS ${CMAKE_SOURCE_DIR}/src/l10n.cc.in
)

list(APPEND TRANSLATIONS_SOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/l10n.cc
)
