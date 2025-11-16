cmake_minimum_required(VERSION 3.10)
project(taranis VERSION 1.14.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(ExternalProject)
include(FeatureSummary)
include(FindPkgConfig)

set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.55.0
  REQUIRED
  COMPONENTS filesystem date_time log log_setup system thread
)

find_library(Curl NAMES curl REQUIRED)
find_library(Inkview NAMES inkview REQUIRED)
find_library(Jsoncpp NAMES jsoncpp REQUIRED)

pkg_check_modules(Opencv REQUIRED opencv)

set(GSL_DIR "${CMAKE_CURRENT_BINARY_DIR}/gsl")
ExternalProject_Add(
  gsl
  URL https://ftp.gnu.org/gnu/gsl/gsl-2.7.1.tar.gz
  URL_HASH SHA256=dcb0fbd43048832b757ff9942691a8dd70026d5da0ff85601e52687f6deeb34b
  DOWNLOAD_NO_PROGRESS ON
  PREFIX "${GSL_DIR}"
  BUILD_IN_SOURCE ON
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} ./configure --prefix=${CMAKE_FIND_ROOT_PATH} --host=arm-obreey-linux-gnueabi --build=x86_64-pc-linux-gnu --target=arm-obreey-linux-gnueabi --enable-static --disable-shared
  BUILD_COMMAND ${CMAKE_COMMAND} -E env make
  INSTALL_COMMAND ""
  LOG_CONFIGURE ON
  LOG_BUILD ON
  LOG_OUTPUT_ON_FAILURE ON
)

include(cmake/generate_about_source.cmake)
include(cmake/generate_icons_source.cmake)
include(cmake/generate_translations_source.cmake)

add_executable(taranis.app
  src/alerts.cc
  src/app.cc
  src/config.cc
  src/convert.cc
  src/currentconditionbox.cc
  src/dailyforecastbox.cc
  src/dailyforecastviewer.cc
  src/events.cc
  src/fonts.cc
  src/forecast.cc
  src/hourlyforecastbox.cc
  src/http.cc
  src/icons.cc
  src/locationbox.cc
  src/locationselector.cc
  src/logging.cc
  src/logo.cc
  src/main.cc
  src/menu.cc
  src/pager.cc
  src/service.cc
  src/stack.cc
  src/state.cc
  src/statusbar.cc
  src/ui.cc
  src/util.cc
  src/version.cc
  ${ABOUT_SOURCES}
  ${ICONS_SOURCES}
  ${TRANSLATIONS_SOURCES}
)
add_dependencies(taranis.app gsl)

target_include_directories(taranis.app
  PRIVATE
  ${GSL_DIR}/src/gsl
)

target_link_libraries(taranis.app
  PRIVATE
  Boost::filesystem
  Boost::date_time
  Boost::log
  Boost::log_setup
  Boost::system
  Boost::thread
  ${Curl}
  ${Inkview}
  ${Jsoncpp}
  "-L${CMAKE_FIND_ROOT_PATH}/usr/lib -lopencv_core -lopencv_highgui -lopencv_imgproc"
  # Don't use ${Opencv_LINK_LIBRARIES} since it adds a dependency to
  # opencv_objdetect, opencv_photo, etc. which aren't installed on the
  # devices
  ${GSL_DIR}/src/gsl/.libs/libgsl.a
  ${GSL_DIR}/src/gsl/cblas/.libs/libgslcblas.a
)

# Installer, archive and checksum targets

configure_file(packaging/_scriptInstall.in _scriptInstall @ONLY)

add_custom_target(populate_installer_tree
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/sinstall/icons"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${CMAKE_BINARY_DIR}/taranis.app"
  "${CMAKE_BINARY_DIR}/sinstall"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${CMAKE_SOURCE_DIR}/icons/icon_taranis.bmp"
  "${CMAKE_SOURCE_DIR}/icons/icon_taranis_f.bmp"
  "${CMAKE_BINARY_DIR}/sinstall/icons"
  COMMENT "Populate installer tree"
)
add_dependencies(populate_installer_tree taranis.app)

add_custom_target(installer
  COMMAND ${CMAKE_SOURCE_DIR}/scripts/generate_installer.sh
  COMMENT "Create installer"
)
add_dependencies(installer populate_installer_tree)

add_custom_target(archive
  COMMAND zip --recurse-paths ../taranis.zip .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/sinstall
  COMMENT "Create archive"
)
add_dependencies(archive populate_installer_tree taranis.app)

add_custom_target(checksum
  COMMAND ${CMAKE_SOURCE_DIR}/scripts/generate_checksum_file.sh
  COMMAND ${CMAKE_COMMAND} -E sha256sum taranis.app taranis.pbi taranis.zip
  COMMENT "Compute checksums"
)
add_dependencies(checksum archive installer taranis.app)

feature_summary(WHAT ALL)
