include(CheckSourceCompiles)
include(CheckSymbolExists)
include(CheckTypeSize)

cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
set(version 0.2.1)
project(
  winprefs
  DESCRIPTION "Dump a registry path to a series of 'reg add' commands."
  HOMEPAGE_URL "https://github.com/Tatsh/winprefs"
  LANGUAGES C
  VERSION ${version})

if((CMAKE_C_COMPILER_ID STREQUAL "MSVC" AND CMAKE_COMPILER_VERSION VERSION_GREATER_EQUAL 19.37.0.0)
   OR CMAKE_C_COMPILER_ID MATCHES "Clang|GNU"
   OR CMAKE_C_COMPILER MATCHES "/winegcc$")
  set(CMAKE_C_STANDARD 23)
endif()

option(BUILD_DOCS "Build documentation." OFF)
option(BUILD_TESTS "Build and run tests." OFF)
option(ENABLE_ASAN "Enable address sanitiser." OFF)
option(ENABLE_COVERAGE "Link tests with gcov." OFF)
option(ENABLE_VLD "Debug only: enable Visual Leak Detector." OFF)
set(TARGET_VERSION
    $<IF:$<CXX_COMPILER_ID:MSVC>,0x600,0x501>
    CACHE STRING "Version of Windows to target in 0x format.")

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
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/winprefs-${version})
endif()

if(NOT MSVC)
  set(CMAKE_REQUIRED_FLAGS "-std=gnu2x")
endif()
set(CMAKE_REQUIRED_DEFINITIONS -D_WIN32_WINNT=${TARGET_VERSION} -DUNICODE -D_UNICODE)
check_symbol_exists(_wcsicmp "wchar.h" HAVE_WCSICMP)
check_symbol_exists(_wcsnicmp "wchar.h" HAVE_WCSNICMP)
check_symbol_exists(EKEYREJECTED "errno.h" HAVE_EKEYREJECTED)
check_type_size(_Bool STDBOOL BUILTIN_TYPES_ONLY LANGUAGE C)
check_type_size(nullptr_t STD_NULLPTR_T BUILTIN_TYPES_ONLY LANGUAGE C)
add_compile_definitions(
  _CRT_SECURE_NO_WARNINGS
  _UNICODE
  HAVE_EKEYREJECTED=$<IF:$<BOOL:${HAVE_EKEYREJECTED}>,1,0>
  HAVE_NULLPTR_T=$<IF:$<BOOL:${HAVE_STD_NULLPTR_T}>,1,0>
  HAVE_STDBOOL=$<IF:$<BOOL:${HAVE_STDBOOL}>,1,0>
  HAVE_WCSICMP=$<IF:$<BOOL:${HAVE_WCSICMP}>,1,0>
  HAVE_WCSNICMP=$<IF:$<BOOL:${HAVE_WCSNICMP}>,1,0>
  UNICODE
  WIN32_LEAN_AND_MEAN)

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
