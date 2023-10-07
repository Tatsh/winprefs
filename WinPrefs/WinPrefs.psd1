@{
  Author = 'Tatsh'
  CompanyName = ''
  CompatiblePSEditions = @('Windows')
  Copyright = '(c) 2023 Tatsh. All rights reserved.'
  Description = 'Convert a registry path to a series of reg commands.'
  GUID = '57c33a4f-3d66-422b-8e2b-596231011201'
  HelpInfoURI = 'https://github.com/Tatsh/winprefs'
  ModuleVersion = '0.1.0'
  PowerShellVersion = '5.1'
  RequiredAssemblies = @()
  RequiredModules = @()
  RootModule = 'WinPrefs'

  AliasesToExport = @('path2reg','prefs-export')
  CmdletsToExport = @()
  FunctionsToExport = @('Save-Preferences','Write-RegCommands')
  VariablesToExport = '*'

  PrivateData = @{
    PSData = @{
      IconUri = 'https://avatars.githubusercontent.com/u/724848?s=96&v=4'
      LicenseUri = 'https://github.com/Tatsh/winprefs/blob/master/LICENSE.txt'
      ProjectUri = 'https://github.com/Tatsh/winprefs'
      ReleaseNotes = 'https://github.com/Tatsh/winprefs/blob/master/CHANGELOG.md'
      Tags = @('admin','administrator','backup','batch','registry','windows')
    }
  }
}
