if(CMAKE_C_COMPILER_ID STREQUAL "MSVC" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 19.37.0.0)
  enable_language(CSharp)
  add_library(
    PSWinPrefs SHARED LibPrefs.cs RegisterSavePreferencesScheduledTask.cs SavePreferences.cs
                      UnregisterSavePreferencesScheduledTask.cs WriteRegCommands.cs)
  set(TASKSCHEDULER_VERSION 2.12.1)
  set_target_properties(
    PSWinPrefs
    PROPERTIES DOTNET_SDK "Microsoft.NET.Sdk"
               DOTNET_TARGET_FRAMEWORK "net9.0"
               VS_GLOBAL_EnforceCodeStyleInBuild True
               VS_GLOBAL_ImplicitUsings enable
               VS_GLOBAL_Nullable enable
               VS_PACKAGE_REFERENCES
               "Microsoft.PowerShell.SDK_7.5.0;TaskScheduler_${TASKSCHEDULER_VERSION}")
  target_compile_options(PSWinPrefs PRIVATE "/unsafe")
  if (BUILD_TESTS)
    target_compile_definitions(PSWinPrefs PRIVATE TESTING=1)
  endif()
  file(TO_CMAKE_PATH $ENV{USERPROFILE} USERPROFILE_DIR)
  set(NUGET_PACKAGES_DIR "${USERPROFILE_DIR}/.nuget/packages")
  install(
    FILES
      WinPrefs.psd1
      "${NUGET_PACKAGES_DIR}/taskscheduler/${TASKSCHEDULER_VERSION}/lib/net9.0-windows7.0/Microsoft.Win32.TaskScheduler.dll"
    DESTINATION WinPrefs)
  install(FILES PSWinPrefs.dll-help.xml DESTINATION WinPrefs/en-US)
  install(TARGETS PSWinPrefs prefs winprefsw DESTINATION WinPrefs)
endif()
