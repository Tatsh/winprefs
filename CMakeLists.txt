cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(
  winprefs
  DESCRIPTION "Dump a registry path to a series of 'reg add' commands."
  HOMEPAGE_URL "https://github.com/Tatsh/winprefs"
  LANGUAGES C
  VERSION 0.2.1)

option(BUILD_DOCS "Build documentation." OFF)
option(BUILD_TESTS "Build and run tests." OFF)
option(ENABLE_COVERAGE "Link tests with gcov." OFF)
option(ENABLE_VLD "Debug only: enable Visual Leak Detector." OFF)

if(BUILD_TESTS)
  find_package(cmocka REQUIRED)
endif()

if(BUILD_DOCS)
  find_package(Doxygen REQUIRED)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${DOXYGEN_OUT} @ONLY)
  add_custom_target(
    doc ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/bincookie-0.1.6)
endif()

add_subdirectory(native)

set(CPACK_PACKAGE_NAME "winprefs")
set(CPACK_PACKAGE_VENDOR "Tatsh")
# FIXME include license file in zip

# FIXME install path should not have version number

# FIXME may need Visual C++ runtime

# FIXME missing license in installer

# FIXME Set publisher

# FIXME Set 'file description'?

# FIXME Set Product name

# FIXME Set 'file version'?

# FIXME Set Product version

# FIXME Set Copyright

# FIXME Set language

# FIXME No UNIX-style packaging. Just have a single root.
set(CPACK_GENERATOR "NSIS;ZIP")
include(CPack)
