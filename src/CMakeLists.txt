if(CMAKE_C_COMPILER_ID STREQUAL "MSVC" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 19.37.0.0)
  enable_language(CSharp)
  add_library(
    PSWinPrefs SHARED LibPrefs.cs RegisterSavePreferencesScheduledTask.cs SavePreferences.cs
                      UnregisterSavePreferencesScheduledTask.cs WriteRegCommands.cs)
  set_target_properties(
    PSWinPrefs
    PROPERTIES DOTNET_SDK "Microsoft.NET.Sdk"
               DOTNET_TARGET_FRAMEWORK "net6.0"
               VS_GLOBAL_EnforceCodeStyleInBuild True
               VS_GLOBAL_ImplicitUsings enable
               VS_GLOBAL_Nullable enable
               VS_PACKAGE_REFERENCES "Microsoft.PowerShell.SDK_7.2.7;TaskScheduler_2.10.1")
  target_compile_options(PSWinPrefs PRIVATE "/unsafe")
  install(FILES WinPrefs.psd1 DESTINATION WinPrefs)
  install(TARGETS PSWinPrefs winprefsw prefs DESTINATION WinPrefs)
endif()
