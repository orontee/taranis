# Generate sources for about dialog

find_package(Git REQUIRED)
execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
  OUTPUT_VARIABLE VCS_SHORT_COMMIT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND ${GIT_EXECUTABLE} describe --tags --match "v[0-9]*.[0-9]*.[0-9]*"
  OUTPUT_VARIABLE VCS_DESCRIBE_COMMIT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(VCS_DESCRIBE_COMMIT STREQUAL "v${taranis_VERSION}")
  set(ABOUT_PROJECT_VERSION "${taranis_VERSION} (${VCS_SHORT_COMMIT})")
else()
  set(ABOUT_PROJECT_VERSION "${taranis_VERSION}-devel (${VCS_DESCRIBE_COMMIT})")
endif()

configure_file(
  src/about.cc.in
  about.cc
  @ONLY
)

unset(ABOUT_PROJECT_VERSION)
unset(VCS_DESCRIBE_COMMIT)
unset(VCS_SHORT_COMMIT)

list(APPEND ABOUT_SOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/about.cc
)
