enable_testing()

set(mocked_functions
    _CrtDumpMemoryLeaks
    _wchdir
    _wfullpath
    _wgetcwd
    _wspawnlp
    wcsicmp
    wCloseHandle
    wCreateDirectoryW
    wCreateFileW
    wCreatePipe
    wCreateProcessW
    wFormatMessageW
    wSHGetFolderPathW
    wGetDateFormatW
    wGetFileAttributes
    wGetLastError
    wGetStdHandle
    wGetTimeFormatW
    wMultiByteToWideChar
    wPathAppendW
    wPathStripPathW
    wPeekNamedPipe
    wReadFile
    wRegCloseKey
    wRegEnumKeyExW
    wRegEnumValueW
    wRegOpenKeyExW
    wRegQueryInfoKeyW
    wRegQueryValueExW
    wWaitForSingleObject
    wWideCharToMultiByte
    wWriteFile)
foreach(func ${mocked_functions})
  list(APPEND test_flags "-Wl,--wrap,${func}")
endforeach()

add_executable(
  winprefs-tests
  ../arg.h
  ../constants.c
  ../constants.h
  ../debug.c
  ../debug.h
  ../git.c
  ../git.h
  ../io.c
  ../io.h
  ../macros.h
  ../main.c
  ../powershell.c
  ../powershell.h
  ../reg_code.c
  ../reg_code.h
  ../reg_command.c
  ../reg_command.h
  ../registry.c
  ../registry.h
  ../shell.c
  ../shell.h
  main.c
  test_main.c
  test_shell.c
  tests.h
  wrappers.c
  wrappers.h)
target_compile_definitions(winprefs-tests PRIVATE TESTING=1)
target_compile_definitions(winprefs-tests PRIVATE _WIN32_WINNT=0x600)
target_compile_options(
  winprefs-tests PRIVATE $<$<CONFIG:Debug>:${GCC_CLANG_DEBUG_C_FLAGS}> ${GCC_CLANG_SHARED_C_FLAGS}
                         -Wno-error=unused-parameter -Wno-unused-parameter)
target_include_directories(winprefs-tests PRIVATE ${CMAKE_SOURCE_DIR}/native)
target_link_libraries(winprefs-tests PRIVATE cmocka)
if(WIN32)
  target_link_options(winprefs-tests PRIVATE -mconsole)
endif()
target_precompile_headers(winprefs-tests PRIVATE ../pch.h)
if(ENABLE_COVERAGE)
  target_compile_options(winprefs-tests PRIVATE --coverage)
  target_link_options(winprefs-tests PRIVATE --coverage)
endif()
target_link_options(winprefs-tests PRIVATE ${test_flags})
add_test(NAME winprefs-tests COMMAND winprefs-tests)
