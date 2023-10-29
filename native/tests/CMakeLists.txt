enable_testing()

set(mocked_functions
    CloseHandle
    CreateProcessW
    GetDateFormatW
    GetFileAttributes
    GetTimeFormatW
    MultiByteToWideChar
    PathAppendW
    PathStripPathW
    PeekNamedPipe
    RegCloseKey
    RegEnumKeyExW
    RegEnumValueW
    RegOpenKeyExW
    RegQueryValueExW
    WaitForSingleObject
    WideCharToMultiByte
    _wspawnlp
    _wfullpath
    calloc
    fclose
    fopen
    fread
    fseek
    ftell
    rewind)
foreach(func ${mocked_functions})
  list(APPEND test_flags "-Wl,--wrap,${func}")
endforeach()

add_executable(
  winprefs-tests
  ../arg.h
  ../constants.c
  ../constants.h
  ../macros.h
  ../reg_command.c
  ../reg_command.h
  ../shell.c
  ../shell.h
  calloc.c
  calloc.h
  cmocka-inc.h
  io.c
  io.h
  main.c
  test_shell.c
  tests.h)
target_compile_definitions(winprefs-tests PRIVATE UNICODE _UNICODE)
get_property(IS_64BIT GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
target_compile_definitions(winprefs-tests PRIVATE _WIN32_WINNT=0x600)
target_compile_options(winprefs-tests PRIVATE $<$<CONFIG:Debug>:${GCC_CLANG_DEBUG_C_FLAGS}>
                                              ${GCC_CLANG_SHARED_C_FLAGS})
target_include_directories(winprefs-tests PRIVATE ${CMAKE_SOURCE_DIR}/native)
target_link_libraries(winprefs-tests PRIVATE kernel32 shlwapi)
target_link_options(winprefs-tests PRIVATE -municode)
if(ENABLE_COVERAGE)
  target_compile_options(winprefs-tests PRIVATE --coverage)
  target_link_options(winprefs-tests PRIVATE --coverage)
endif()
target_link_options(winprefs-tests PRIVATE ${test_flags})
add_test(NAME winprefs-tests COMMAND winprefs-tests)
