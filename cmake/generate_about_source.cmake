# Generate sources for about dialog

find_package(Git REQUIRED)
execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
  OUTPUT_VARIABLE VCS_TAG
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT VCS_TAG)
  set(VCS_TAG "devel")
endif()

configure_file(
  src/about.cc.in
  about.cc
  @ONLY
)

list(APPEND ABOUT_SOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/about.cc
)
