if(CMAKE_GENERATOR MATCHES "Visual Studio")
  enable_language(CSharp)
  add_library(WinPrefs LibPrefs.cs RegisterSavePreferencesScheduledTask.cs SavePreferences.cs
                       UnregisterSavePreferencesScheduledTask.cs WriteRegCommands.cs)
  set_property(TARGET WinPrefs PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v6.0")
  set_property(TARGET WinPrefs PROPERTY VS_PACKAGE_REFERENCES "Microsoft.PowerShell.SDK_7.2.7")
endif()
