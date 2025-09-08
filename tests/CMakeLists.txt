enable_testing()
if(CMAKE_C_COMPILER_ID STREQUAL "MSVC" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 19.37.0.0)
  enable_language(CSharp)
  add_library(
    PSWinPrefsTests SHARED
    LibPrefsTest.cs PowershellEmulator.cs RegisterSavePreferencesScheduledTaskTest.cs
    SavePreferencesTest.cs UnregisterSavePreferencesScheduledTaskTest.cs WriteRegCommandsTest.cs)
  target_link_libraries(PSWinPrefsTests PRIVATE PSWinPrefs)
  set_target_properties(
    PSWinPrefsTests
    PROPERTIES
      DOTNET_SDK "Microsoft.NET.Sdk"
      DOTNET_TARGET_FRAMEWORK "net9.0"
      VS_GLOBAL_DebugType "Full"
      VS_GLOBAL_EnforceCodeStyleInBuild True
      VS_GLOBAL_ImplicitUsings enable
      VS_GLOBAL_Nullable enable
      VS_GLOBAL_Packable "false"
      VS_GLOBAL_TestProjectType "UnitTest"
      VS_GLOBAL_PROJECT_TYPES
      "{3AC096D0-A1C2-E12C-1390-A8335801FDAB};{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}"
      VS_PACKAGE_REFERENCES
      "Microsoft.NET.Test.Sdk_17.14.1;MSTest.TestAdapter_3.10.4;MSTest.TestFramework_3.10.4;Moq_4.20.72;xunit_2.9.3;xunit.runner.visualstudio_3.1.1"
  )
  add_custom_command(
    TARGET PSWinPrefsTests
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:prefs>
            $<TARGET_FILE_DIR:PSWinPrefsTests>
    VERBATIM)
  # https://github.com/microsoft/vstest/blob/main/docs/analyze.md#collect-coverage-with-command-line-runner
  add_test(NAME PSWinPrefsTests
           COMMAND "$ENV{VSINSTALLDIR}\\Common7\\IDE\\Extensions\\TestPlatform\\vstest.console.exe"
                   "$<TARGET_FILE:PSWinPrefsTests>")
endif()
