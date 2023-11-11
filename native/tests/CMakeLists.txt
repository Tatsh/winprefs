enable_testing()
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")

  set(test_linker_flags $<$<BOOL:${WIN32}>:-mconsole> $<$<BOOL:${ENABLE_COVERAGE}>:--coverage>)
  set(mocked_functions
      CloseHandle
      CreateDirectory
      CreateFile
      CreatePipe
      CreateProcess
      FormatMessage
      GetDateFormat
      GetFileAttributes
      GetLastError
      GetStdHandle
      GetTimeFormat
      MultiByteToWideChar
      PathAppend
      PathStripPath
      PeekNamedPipe
      ReadFile
      RegCloseKey
      RegEnumKeyEx
      RegEnumValue
      RegOpenKeyEx
      RegQueryInfoKey
      RegQueryValueEx
      SHGetFolderPath
      WaitForSingleObject
      WideCharToMultiByte
      WriteFile
      _CrtDumpMemoryLeaks
      _wchdir
      _wfullpath
      _wgetcwd
      _wspawnlp)
  foreach(func ${mocked_functions})
    list(APPEND test_linker_flags "-Wl,--wrap,${func}")
  endforeach()
  set(test_c_defs TESTING=1 _WIN32_WINNT=0x600)
  set(test_c_flags
      $<$<CONFIG:Debug>:${GCC_CLANG_DEBUG_C_FLAGS}>
      ${GCC_CLANG_SHARED_C_FLAGS}
      -Wno-error=unused-function
      -Wno-error=unused-parameter
      -Wno-unused-function
      -Wno-unused-parameter
      $<$<BOOL:${ENABLE_COVERAGE}>:--coverage>)
  set(test_include_dirs ${CMAKE_SOURCE_DIR}/native)
  set(test_pch ../pch.h)

  add_library(
    tests
    ../arg.h
    ../constants.c
    ../constants.h
    ../debug.c
    ../debug.h
    ../git.c
    ../git.h
    ../git_branch.c
    ../git_branch.h
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
    wrappers.c
    wrappers.h)
  target_compile_definitions(tests PRIVATE ${test_c_defs})
  target_compile_options(tests PRIVATE ${test_c_flags} $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>)
  target_include_directories(tests PRIVATE ${test_include_dirs})
  target_link_options(tests PRIVATE ${test_linker_flags})
  target_precompile_headers(tests PRIVATE ${test_pch})

  set(test_categories get_git_branch git_commit main powershell save_prefs shell)
  foreach(category ${test_categories})
    add_executable(test-${category} test_${category}.c)
    target_compile_definitions(test-${category} PRIVATE ${test_c_defs})
    target_compile_options(test-${category} PRIVATE ${test_c_flags})
    target_include_directories(test-${category} PRIVATE ${test_include_dirs})
    target_link_libraries(test-${category}
                          PRIVATE tests $<IF:$<TARGET_EXISTS:cmocka::cmocka>,cmocka::cmocka,cmocka>)
    target_link_options(
      test-${category}
      PRIVATE
      ${test_linker_flags}
      $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>
      $<$<STREQUAL:git_commit,${category}>:-Wl,--wrap,get_git_branch>
      $<$<STREQUAL:main,${category}>:-Wl,--wrap,save_preferences
      -Wl,--wrap,export_single_value>
      $<$<STREQUAL:powershell,${category}>:-Wl,--wrap,write_output>)
    target_precompile_headers(test-${category} PRIVATE ${test_pch})

    add_test(NAME "${category} tests" COMMAND test-${category})
  endforeach()
endif()
