@{
  Author                 = 'Tatsh'
  CompanyName            = ''
  CompatiblePSEditions   = @('Windows')
  Copyright              = '(c) 2023 Tatsh. All rights reserved.'
  Description            = 'Convert a registry path to a series of reg commands and more.'
  DotNetFrameworkVersion = '6.0'
  GUID                   = '57c33a4f-3d66-422b-8e2b-596231011201'
  HelpInfoURI            = 'https://github.com/Tatsh/winprefs'
  ModuleVersion          = '0.3.0'
  PowerShellVersion      = '5.1'
  ProcessorArchitecture  = 'Amd64'
  NestedModules          = @()
  RequiredAssemblies     = @()
  RequiredModules        = @()
  RootModule             = 'PSWinPrefs.dll'

  AliasesToExport        = @('path2reg', 'prefs-export', 'winprefs-install-job',
    'winprefs-uninstall-job')
  CmdletsToExport        = @('Register-SavePreferencesScheduledTask', 'Save-Preferences',
    'Unregister-SavePreferencesScheduledTask', 'Write-RegCommands')
  FunctionsToExport      = @()
  VariablesToExport      = '*'

  PrivateData            = @{
    PSData = @{
      IconUri      = 'https://avatars.githubusercontent.com/u/724848?s=96&v=4'
      LicenseUri   = 'https://github.com/Tatsh/winprefs/blob/master/LICENSE.txt'
      ProjectUri   = 'https://github.com/Tatsh/winprefs'
      ReleaseNotes = 'https://github.com/Tatsh/winprefs/blob/master/CHANGELOG.md'
      Tags         = @('admin', 'administrator', 'backup', 'batch', 'registry', 'windows')
    }
  }
}
