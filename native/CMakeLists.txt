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

function(add_vld target)
  target_compile_definitions(
    ${target}
    PRIVATE
      $<$<AND:$<BOOL:${WITH_VLD}>,$<OR:$<STREQUAL:CMAKE_BUILD_TYPE,Debug>,$<STREQUAL:CMAKE_BUILD_TYPE,RelWithDebInfo>>>:ENABLE_VLD>
  )
  target_include_directories(
    ${target}
    PRIVATE
      $<$<AND:$<BOOL:${WITH_VLD}>,$<OR:$<STREQUAL:CMAKE_BUILD_TYPE,Debug>,$<STREQUAL:CMAKE_BUILD_TYPE,RelWithDebInfo>>>:${VLD_H}>
  )
  target_link_libraries(
    ${target}
    PRIVATE
      $<$<AND:$<BOOL:${WITH_VLD}>,$<OR:$<STREQUAL:CMAKE_BUILD_TYPE,Debug>,$<STREQUAL:CMAKE_BUILD_TYPE,RelWithDebInfo>>>:${VLD}>
  )
endfunction()

function(add_min_winnt target)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU" OR CMAKE_C_COMPILER MATCHES "/winegcc$")
    get_property(IS_64BIT GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
    # mingw can do 2000 without extra help. 64-bit requires at least XP.
    if(IS_64BIT AND NOT TARGET_VERSION MATCHES "^(0x501|0x6)")
      message(
        FATAL_ERROR
          "Invalid version for 64-bit. Must be XP (0x501) or higher. Version: ${TARGET_VERSION}")
    endif()
    target_compile_definitions(${target} PRIVATE _WIN32_WINNT=${TARGET_VERSION})
  else() # MSVC gets Vista+
    target_compile_definitions(${target} PRIVATE _WIN32_WINNT=${TARGET_VERSION})
  endif()
endfunction()

function(add_asan target)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU" OR CMAKE_C_COMPILER MATCHES "/winegcc$")
    target_compile_definitions(${target} PRIVATE $<$<BOOL:${ENABLE_ASAN}>:ASAN_ENABLED>)
    target_compile_options(${target} PRIVATE $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>)
    target_link_options(${target} PRIVATE $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>)
  endif()
endfunction()

function(add_ubsan target)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU" OR CMAKE_C_COMPILER MATCHES "/winegcc$")
    target_compile_definitions(${target} PRIVATE $<$<BOOL:${ENABLE_UBSAN}>:UBSAN_ENABLED>)
    target_compile_options(${target} PRIVATE $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>)
    target_link_options(${target} PRIVATE $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>)
  endif()
endfunction()

function(add_flags target)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU" OR CMAKE_C_COMPILER MATCHES "/winegcc$")
    target_compile_options(${target} PRIVATE ${GCC_CLANG_SHARED_C_FLAGS}
                                             $<$<CONFIG:Debug>:${GCC_CLANG_DEBUG_C_FLAGS}>)
  else()
    target_compile_options(${target} PRIVATE /Zc:forScope /Zc:inline /Zc:wchar_t /external:W0
                                             /utf-8)
  endif()
endfunction()

if(WIN32)
  set(prefs_src
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
      io_default_writer.c
      io_default_writer.h
      macros.h
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

  add_library(prefs-obj OBJECT ${prefs_src})
  target_precompile_headers(prefs-obj PRIVATE pch.h)
  add_asan(prefs-obj)
  add_ubsan(prefs-obj)
  add_flags(prefs-obj)
  add_min_winnt(prefs-obj)
  add_vld(prefs-obj)

  add_library(prefs SHARED $<TARGET_OBJECTS:prefs-obj>)
  target_precompile_headers(prefs PRIVATE pch.h)
  target_link_libraries(prefs PRIVATE shlwapi)
  add_asan(prefs)
  add_ubsan(prefs)
  add_flags(prefs)
  add_min_winnt(prefs)
  add_vld(prefs)

  set(winprefs_targets winprefs winprefsw)
  foreach(exe ${winprefs_targets})
    add_executable(${exe} $<TARGET_OBJECTS:prefs-obj> main.c)
    target_precompile_headers(${exe} PRIVATE pch.h)
    target_link_libraries(${exe} PRIVATE shlwapi)
    add_asan(${exe})
    add_ubsan(${exe})
    add_flags(${exe})
    add_min_winnt(${exe})
    add_vld(${exe})
    if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU" OR CMAKE_C_COMPILER MATCHES "/winegcc$")
      target_link_libraries(${exe} PRIVATE shell32)
      target_link_options(${exe} PRIVATE $<IF:$<STREQUAL:winprefs,${exe}>,-mconsole,-mwindows>
                          -municode)
      if(CMAKE_C_COMPILER MATCHES "/winegcc$")
        target_include_directories(${exe} PRIVATE /usr/include/wine/msvcrt)
        target_link_libraries(${exe} PRIVATE ucrtbase)
      endif()
    else()
      target_link_libraries(${exe} PRIVATE kernel32)
      target_link_options(
        ${exe}
        PRIVATE
        /DYNAMICBASE
        /NXCOMPAT
        /TLBID:1
        $<$<STREQUAL:winprefsw,${exe}>:/ENTRY:wmainCRTStartup
        /SUBSYSTEM:WINDOWS>)
    endif()
    install(TARGETS ${exe} DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES ../winprefs.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/1)
  endforeach()

endif()

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
