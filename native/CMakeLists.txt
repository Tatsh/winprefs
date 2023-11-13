include(GNUInstallDirs)

if(CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo" AND WITH_VLD)
  find_library(VLD vld HINTS "C:/Program Files (x86)/Visual Leak Detector/lib/Win64"
                             "C:/Program Files/Visual Leak Detector/lib/Win64" REQUIRED)
  find_path(VLD_H vld.h HINTS "C:/Program Files (x86)/Visual Leak Detector/include"
                              "C:/Program Files/Visual Leak Detector/include" REQUIRED)
endif()

set(GCC_CLANG_SHARED_C_FLAGS -fno-builtin)
set(GCC_CLANG_DEBUG_C_FLAGS
    -ggdb
    -O0
    -pedantic
    -Wall
    -Wdeprecated-declarations
    -Wempty-body
    -Werror
    -Werror=conversion
    -Werror=float-conversion
    -Werror=implicit-function-declaration
    -Werror=incompatible-pointer-types
    -Werror=int-conversion
    -Werror=return-type
    -Werror=sign-conversion
    -Wmissing-braces
    -Wmissing-field-initializers
    -Wno-error=unused-value
    -Wno-missing-prototypes
    -Wno-strict-prototypes
    -Wno-trigraphs
    -Wno-unknown-pragmas
    -Wno-unused-value
    -Wparentheses
    -Wpointer-sign
    -Wshadow
    -Wsign-compare
    -Wswitch
    -Wuninitialized
    -Wunknown-pragmas
    -Wunreachable-code
    -Wunused-function
    -Wunused-label
    -Wunused-parameter
    -Wunused-variable)

if(WIN32)
  add_executable(
    winprefs
    arg.h
    constants.c
    constants.h
    debug.c
    debug.h
    git.c
    git.h
    git_branch.c
    git_branch.h
    io.c
    io.h
    macros.h
    main.c
    powershell.c
    powershell.h
    reg_code.c
    reg_code.h
    reg_command.c
    reg_command.h
    registry.c
    registry.h
    shell.c
    shell.h
    utils.h)
  target_precompile_headers(winprefs PRIVATE pch.h)
  target_link_libraries(winprefs PRIVATE shlwapi)
  target_compile_definitions(
    winprefs
    PRIVATE
      $<$<AND:$<BOOL:${WITH_VLD}>,$<OR:$<STREQUAL:CMAKE_BUILD_TYPE,Debug>,$<STREQUAL:CMAKE_BUILD_TYPE,RelWithDebInfo>>>:ENABLE_VLD>
  )
  target_include_directories(
    winprefs
    PRIVATE
      $<$<AND:$<BOOL:${WITH_VLD}>,$<OR:$<STREQUAL:CMAKE_BUILD_TYPE,Debug>,$<STREQUAL:CMAKE_BUILD_TYPE,RelWithDebInfo>>>:${VLD_H}>
  )
  target_link_libraries(
    winprefs
    PRIVATE
      $<$<AND:$<BOOL:${WITH_VLD}>,$<OR:$<STREQUAL:CMAKE_BUILD_TYPE,Debug>,$<STREQUAL:CMAKE_BUILD_TYPE,RelWithDebInfo>>>:${VLD}>
  )
  if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU" OR CMAKE_C_COMPILER MATCHES "/winegcc$")
    get_property(IS_64BIT GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
    # mingw can do 2000 without extra help. 64-bit requires at least XP.
    if(IS_64BIT AND NOT TARGET_VERSION MATCHES "^(0x501|0x6)")
      message(
        FATAL_ERROR
          "Invalid version for 64-bit. Must be XP (0x501) or higher. Version: ${TARGET_VERSION}")
    endif()
    target_compile_definitions(winprefs PRIVATE _WIN32_WINNT=${TARGET_VERSION})
    target_compile_options(
      winprefs PRIVATE ${GCC_CLANG_SHARED_C_FLAGS} $<$<CONFIG:Debug>:${GCC_CLANG_DEBUG_C_FLAGS}>
                       $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>)
    target_link_libraries(winprefs PRIVATE shell32)
    target_link_options(winprefs PRIVATE -mconsole -municode
                        $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>)
    if(CMAKE_C_COMPILER MATCHES "/winegcc$")
      target_include_directories(winprefs PRIVATE /usr/include/wine/msvcrt)
      target_link_libraries(winprefs PRIVATE ucrtbase)
    endif()
  else() # MSVC gets Vista+
    target_compile_definitions(winprefs PRIVATE _WIN32_WINNT=${TARGET_VERSION})
    target_compile_options(winprefs PRIVATE /Zc:forScope /Zc:inline /Zc:wchar_t /external:W0 /utf-8)
    target_link_libraries(winprefs PRIVATE kernel32)
    target_link_options(winprefs PRIVATE /DYNAMICBASE /NXCOMPAT /TLBID:1)
  endif()
  install(TARGETS winprefs DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
