include(CheckSourceCompiles)
include(CheckSymbolExists)
include(CheckTypeSize)

cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
set(version 0.3.4)
project(
  winprefs
  DESCRIPTION "Dump a registry path to a series of 'reg add' commands."
  HOMEPAGE_URL "https://github.com/Tatsh/winprefs"
  LANGUAGES C
  VERSION ${version})

if((CMAKE_C_COMPILER_ID STREQUAL "MSVC" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 19.37.0.0
   ) OR CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  set(CMAKE_C_STANDARD 23)
  if(WIN32 AND NOT ("$ENV{USERPROFILE}" STREQUAL ""))
    file(TO_CMAKE_PATH $ENV{USERPROFILE} USERPROFILE_DIR)
    set(NUGET_PACKAGES_DIR "${USERPROFILE_DIR}/.nuget/packages")
  endif()
endif()

option(BUILD_CSharp_TESTS "Build C# tests." OFF)
option(BUILD_DOCS "Build documentation." OFF)
option(BUILD_TESTS "Build tests." OFF)
option(ENABLE_ASAN "Enable address sanitiser." OFF)
option(ENABLE_COVERAGE "Link tests with gcov." OFF)
option(ENABLE_UBSAN "Enable undefined behaviour sanitiser." OFF)
option(ENABLE_VLD "Debug only: enable Visual Leak Detector." OFF)
set(TARGET_VERSION
    $<IF:$<CXX_COMPILER_ID:MSVC>,0x600,0x501>
    CACHE STRING "Version of Windows to target in 0x format.")

if(BUILD_TESTS)
  find_package(cmocka REQUIRED)
endif()

if(BUILD_DOCS)
  set(DOXYGEN_GENERATE_TAGFILE "${CMAKE_BINARY_DIR}/html/${CMAKE_PROJECT_NAME}.tags")
  set(DOXYGEN_INTERACTIVE_SVG YES)
  set(DOXYGEN_MARKDOWN_ID_STYLE GITHUB)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")
  find_package(Doxygen REQUIRED COMPONENTS dot)
  doxygen_add_docs(docs README.md native src ALL)
endif()

if(NOT MSVC)
  set(CMAKE_REQUIRED_FLAGS "-std=gnu2x")
endif()
set(CMAKE_REQUIRED_DEFINITIONS -D_WIN32_WINNT=${TARGET_VERSION} -DUNICODE -D_UNICODE)
check_symbol_exists(_wcsicmp "wchar.h" HAVE_WCSICMP)
check_symbol_exists(_wcsnicmp "wchar.h" HAVE_WCSNICMP)
check_symbol_exists(EKEYREJECTED "errno.h" HAVE_EKEYREJECTED)
check_type_size(wchar_t SIZEOF_WCHAR_T LANGUAGE C)
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
  SIZEOF_WCHAR_T=${SIZEOF_WCHAR_T}
  UNICODE
  WIN32_LEAN_AND_MEAN)

add_subdirectory(native)
add_subdirectory(src)
if(BUILD_CSharp_TESTS)
  add_subdirectory(tests)
endif()

set(CPACK_PACKAGE_NAME "winprefs")
set(CPACK_PACKAGE_VENDOR "Tatsh")
set(CPACK_GENERATOR "NSIS;ZIP")
include(CPack)
