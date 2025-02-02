Import-Module Pester
Import-Module WinPrefs

Describe "RegisterSavePreferencesScheduledTask" {
  Context "F" {
  BeforeAll {
  Mock -ModuleName "Microsoft.Win32.TaskScheduler" "TaskService" {
    return [PSCustomObject]@{
      RootFolder = [PSCustomObject]@{
        CreateFolder = { param($name, $securityDescriptor, $createIfMissing) return $null }
      }
      NewTask = { return [PSCustomObject]@{
        RegistrationInfo = [PSCustomObject]@{ Description = $null }
        Triggers = [System.Collections.Generic.List[object]]::new()
        Settings = [PSCustomObject]@{
          ExecutionTimeLimit = $null
          StartWhenAvailable = $null
        }
        Actions = [System.Collections.Generic.List[object]]::new()
      }}
    }
  }

  Mock -ModuleName "Microsoft.Win32.TaskScheduler" "TaskFolder" {
    return [PSCustomObject]@{
      RegisterTaskDefinition = { param($name, $taskDefinition) return $null }
    }
  }

  Mock -ModuleName "Microsoft.Win32.TaskScheduler" "ExecAction" {
    param($path, $arguments, $workingDirectory)
    return [PSCustomObject]@{
      Path = $path
      Arguments = $arguments
      WorkingDirectory = $workingDirectory
    }
  }
  }

  It "should create a scheduled task with correct properties" {
    $task = @{}
    $task.MaxDepth = 10
    $task.Commit = $true
    $task.DeployKey = "path\to\deploy-key"
    $task.OutputDirectory = "path\to\output"
    $task.OutputFile = "output.bat"
    $task.Format = "ps1"
    $task.Path = "HKCU:\Software"

    Register-SavePreferencesScheduledTask @task

    Assert-MockCalled -ModuleName "Microsoft.Win32.TaskScheduler" "TaskService" `
      -Exactly 1
    Assert-MockCalled -ModuleName "Microsoft.Win32.TaskScheduler" "TaskFolder" `
      -Exactly 1
    Assert-MockCalled -ModuleName "Microsoft.Win32.TaskScheduler" "ExecAction" `
      -Exactly 1

    $taskDefinition = (Get-MockCalled -ModuleName "Microsoft.Win32.TaskScheduler" `
      "TaskService").NewTask()
    $taskDefinition.RegistrationInfo.Description | Should `
      -Be "Runs WinPrefs every 12 hours (path HKCU:\Software)."
    $taskDefinition.Triggers.Count | Should -Be 1
    $taskDefinition.Settings.ExecutionTimeLimit | Should -Be ([TimeSpan]::FromHours(2))
    $taskDefinition.Settings.StartWhenAvailable | Should -Be $true
    $taskDefinition.Actions.Count | Should -Be 1
  }
  }
}
