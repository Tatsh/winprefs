Import-Module Pester
Import-Module WinPrefs

Describe "UnregisterSavePreferencesScheduledTask" {
  It "Deletes the tat.sh folder if it is empty" {
    $cmdlet = [WinPrefs.UnregisterSavePreferencesScheduledTask]::new()
    $cmdlet.ProcessRecord()
    Assert-MockCalled -ModuleName Microsoft.Win32.TaskScheduler -MemberName 'DeleteFolder' `
      -Exactly 1 `
      -ParameterFilter { $_ -eq "tat.sh" }
  }

  It "Does not delete the task if it does not exist" {
    Mock -ModuleName Microsoft.Win32.TaskScheduler -MemberName 'TaskService' -MockWith {
      Mock -MemberName 'RootFolder' -MockWith {
        return @{
          SubFolders = @{
            Exists = { param($name) return $name -eq "tat.sh" }
            "tat.sh" = @{
              SubFolders = @{
                Exists = { param($name) return $name -eq "WinPrefs" }
                "WinPrefs" = @{
                  Tasks = @{
                    Count = 0
                  }
                  DeleteTask = { param($name) }
                  DeleteFolder = { param($name) }
                }
              }
              Tasks = @{
                Count = 0
              }
              DeleteFolder = { param($name) }
            }
          }
        }
      }
    }
    $cmdlet = [WinPrefs.UnregisterSavePreferencesScheduledTask]::new()
    $cmdlet.ProcessRecord()
    Assert-MockCalled -ModuleName Microsoft.Win32.TaskScheduler -MemberName 'DeleteTask' `
      -Exactly 0
  }

  It "Deletes the WinPrefs folder if it is empty" {
    Mock -ModuleName Microsoft.Win32.TaskScheduler -MemberName 'TaskService' -MockWith {
      Mock -MemberName 'RootFolder' -MockWith {
        return @{
          SubFolders = @{
            Exists = { param($name) return $name -eq "tat.sh" }
            "tat.sh" = @{
              SubFolders = @{
                Exists = { param($name) return $name -eq "WinPrefs" }
                "WinPrefs" = @{
                  Tasks = @{
                    Count = 0
                  }
                  DeleteTask = { param($name) }
                  DeleteFolder = { param($name) }
                }
              }
              Tasks = @{
                Count = 0
              }
              DeleteFolder = { param($name) }
            }
          }
        }
      }
    }
    $cmdlet = [WinPrefs.UnregisterSavePreferencesScheduledTask]::new()
    $cmdlet.ProcessRecord()
    Assert-MockCalled -ModuleName Microsoft.Win32.TaskScheduler -MemberName 'DeleteFolder' `
      -Exactly 1 `
      -ParameterFilter { $_ -eq "WinPrefs" }
  }
}
