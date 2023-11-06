enable_testing()
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  set(mocked_functions
      _CrtDumpMemoryLeaks
      _wchdir
      _wfullpath
      _wgetcwd
      _wspawnlp
      wcsicmp
      CloseHandle
      CreateDirectory
      CreateFile
      CreatePipe
      CreateProcess
      FormatMessage
      SHGetFolderPath
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
      WaitForSingleObject
      WideCharToMultiByte
      WriteFile)
  set(categories main shell)
  foreach(func ${mocked_functions})
    list(APPEND test_flags "-Wl,--wrap,${func}")
  endforeach()
  foreach(category ${categories})
    add_executable(
      test-${category}
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
      test_${category}.c
      wrappers.c
      wrappers.h)
    target_compile_definitions(test-${category} PRIVATE TESTING=1 _WIN32_WINNT=0x600)
    target_compile_options(
      test-${category}
      PRIVATE $<$<CONFIG:Debug>:${GCC_CLANG_DEBUG_C_FLAGS}> ${GCC_CLANG_SHARED_C_FLAGS}
              -Wno-error=unused-parameter -Wno-unused-parameter
              $<$<BOOL:${ENABLE_COVERAGE}>:--coverage>)
    target_include_directories(test-${category} PRIVATE ${CMAKE_SOURCE_DIR}/native)
    target_link_libraries(test-${category}
                          PRIVATE $<IF:$<TARGET_EXISTS:cmocka::cmocka>,cmocka::cmocka,cmocka>)
    target_link_options(test-${category} PRIVATE ${test_flags} $<$<BOOL:${WIN32}>:-mconsole>
                        $<$<BOOL:${ENABLE_COVERAGE}>:--coverage>)
    target_precompile_headers(test-${category} PRIVATE ../pch.h)
    add_test(NAME "${category} tests" COMMAND test-${category})
  endforeach()
endif()
