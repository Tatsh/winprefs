Import-Module Pester
Import-Module WinPrefs

Describe "SavePreferences Tests" {
  Mock -ModuleName LibPrefs -MemberName SetDebugPrintEnabled -MockWith {}

  Mock -ModuleName LibPrefs -MemberName SavePreferences -MockWith {
    param (
      $topKey, $writeObject, $writeStdOut, $commit, $deployKey, $outputDirectory, $outputFile,
      $maxDepth, $path, $format
    )
    return $false
  }

  Mock -ModuleName LibPrefs -MemberName GetTopKey -MockWith {
    param ($path)
    return [Microsoft.Win32.Registry]::CurrentUser
  }

  It "should throw terminating error when SavePreferences fails" {
    $cmdlet = [WinPrefs.SavePreferences]::new()
    $cmdlet.Path = "HKCU:\\Software\\Test"
    $cmdlet.Format = "reg"
    $cmdlet.MaxDepth = 20
    $cmdlet.OutputDirectory = "C:\\TestOutput"
    $cmdlet.OutputFile = "test.bat"
    { $cmdlet.ProcessRecord() } | Should -Throw -ErrorId "WinPrefs_SavePreferencesError"
  }

  It "should not throw error when SavePreferences succeeds" {
    Mock -ModuleName LibPrefs -MemberName SavePreferences -MockWith {
      param (
        $topKey, $writeObject, $writeStdOut, $commit, $deployKey, $outputDirectory, $outputFile,
        $maxDepth, $path, $format
      )
      return $true
    }
    $cmdlet = [WinPrefs.SavePreferences]::new()
    $cmdlet.Path = "HKCU:\\Software\\Test"
    $cmdlet.Format = "reg"
    $cmdlet.MaxDepth = 20
    $cmdlet.OutputDirectory = "C:\\TestOutput"
    $cmdlet.OutputFile = "test.bat"
    { $cmdlet.ProcessRecord() } | Should -Not -Throw
  }

  It "should set debug mode when Debug parameter is specified" {
    $cmdlet = [WinPrefs.SavePreferences]::new()
    $cmdlet.Path = "HKCU:\\Software\\Test"
    $cmdlet.Format = "reg"
    $cmdlet.MaxDepth = 20
    $cmdlet.OutputDirectory = "C:\\TestOutput"
    $cmdlet.OutputFile = "test.bat"
    $cmdlet.MyInvocation.BoundParameters["Debug"] = $true
    $cmdlet.BeginProcessing()
    Assert-MockCalled -ModuleName LibPrefs -MemberName SetDebugPrintEnabled -Exactly 1
  }

  It "should not set debug mode when Debug parameter is not specified" {
    $cmdlet = [WinPrefs.SavePreferences]::new()
    $cmdlet.Path = "HKCU:\\Software\\Test"
    $cmdlet.Format = "reg"
    $cmdlet.MaxDepth = 20
    $cmdlet.OutputDirectory = "C:\\TestOutput"
    $cmdlet.OutputFile = "test.bat"
    $cmdlet.BeginProcessing()
    Assert-MockCalled -ModuleName LibPrefs -MemberName SetDebugPrintEnabled -Exactly 0
  }
}
