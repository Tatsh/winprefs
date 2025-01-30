Import-Module Pester
Import-Module WinPrefs

Describe "WriteRegCommands Tests" {
  Mock -ModuleName Microsoft.Win32 -MemberName GetTopKey -MockWith {
    param ($path)
    return [Microsoft.Win32.Registry]::CurrentUser
  }

  Mock -ModuleName Microsoft.Win32 -MemberName OpenSubKey -MockWith {
    param ($subkey)
    return $null
  }

  Mock -ModuleName LibPrefs -MemberName ExportSingleValue -MockWith {
    param ($topKey, $path, $writeObject, $format)
    return $false
  }

  Mock -ModuleName LibPrefs -MemberName SavePreferences -MockWith {
    param ($format, $hk, $maxDepth, $outputFile, $specifiedPath, $writeObjectIn, $writeStdOut)
    return $false
  }

  It "should throw terminating error when ExportSingleValue fails" {
    $cmdlet = [WinPrefs.WriteRegCommands]::new()
    $cmdlet.Path = "HKCU:\\Software\\Test"
    $cmdlet.Format = "reg"
    $cmdlet.MaxDepth = 20
    { $cmdlet.ProcessRecord() } | Should -Throw -ErrorId "WinPrefs_ExportSingleValueError"
  }

  It "should throw terminating error when SavePreferences fails" {
    Mock -ModuleName Microsoft.Win32 -MemberName OpenSubKey -MockWith {
      param ($subkey)
      return [Microsoft.Win32.Registry]::CurrentUser
    }
    $cmdlet = [WinPrefs.WriteRegCommands]::new()
    $cmdlet.Path = "HKCU:\\Software\\Test"
    $cmdlet.Format = "reg"
    $cmdlet.MaxDepth = 20
    { $cmdlet.ProcessRecord() } | Should -Throw -ErrorId "WinPrefs_WriteRegCommandsError"
  }

  It "should not throw error when SavePreferences succeeds" {
    Mock -ModuleName LibPrefs -MemberName SavePreferences -MockWith {
      param ($format, $hk, $maxDepth, $outputFile, $specifiedPath, $writeObjectIn, $writeStdOut)
      return $true
    }
    $cmdlet = [WinPrefs.WriteRegCommands]::new()
    $cmdlet.Path = "HKCU:\\Software\\Test"
    $cmdlet.Format = "reg"
    $cmdlet.MaxDepth = 20
    { $cmdlet.ProcessRecord() } | Should -Not -Throw
  }
}
