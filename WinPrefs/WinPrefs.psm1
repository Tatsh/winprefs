function ReplaceFullHiveNameWithShortName {
  param(
    [Parameter(Mandatory,HelpMessage = "Registry path with long name and no drive syntax.")]
    [string]$Path)
  $_unused,$HkeyParts = $Path.ToUpper().Split('\')[0].Split('_')
  $Path -replace '^HKEY_[^\\]+\\',"HK$($(foreach ($Item in $HkeyParts) { $Item[0] }) -Join ''):"
}

function GetFullHiveName {
  param(
    [Parameter(Mandatory,HelpMessage = "Registry path.")]
    [ValidatePattern('^HK(LM|CU|CR|U|CC|PD):')]
    [string]$Path
  )
  switch -Wildcard ($Path) {
    'HKCC:*' { 'HKEY_CURRENT_CONFIG' }
    'HKCR:*' { 'HKEY_CLASSES_ROOT' }
    'HKCU:*' { 'HKEY_CURRENT_USER' }
    'HKLM:*' { 'HKEY_LOCAL_MACHINE' }
    'HKU:*' { 'HKEY_USERS' }
    default { throw }
  }
}

function GetRegType () {
  param(
    [Parameter(Mandatory)]
    [AllowNull()]
    [ValidatePattern('^(Binary|(D|Q)Word|(Multi|Expand)String|String|None)')]
    $Value
  )
  if ($null -eq $Value) {
    return 'REG_NONE'
  }
  switch ($Value) {
    'Binary' { 'REG_BINARY' }
    'DWord' { 'REG_DWORD' }
    'ExpandString' { 'REG_EXPAND_SZ' }
    'MultiString' { 'REG_MULTI_SZ' }
    'None' { 'REG_NONE' }
    'QWord' { 'REG_QWORD' }
    'String' { 'REG_SZ' }
    default { throw "$Value" }
  }
}

function FixVParameter {
  param(
    [Parameter(Mandatory)]
    [string]$Prop
  )
  if ($Prop -eq '(default)') {
    '/ve '
  }
  else {
    "/v ""$(Escape $Prop)"" "
  }
}

function Escape {
  param(
    [Parameter(Mandatory)]
    [AllowNull()]
    [AllowEmptyString()]
    [string]$Value
  )
  if ($null -eq $Value) {
    return ""
  }
  $Value -replace '"','""' -replace '%','%%'
}

function ConvertValueForReg {
  param(
    [Parameter(Mandatory)]
    [ValidatePattern('^REG_(BINARY|(?:Q|D)WORD|(?:(?:EXPAND|MULTI)_)?SZ|NONE)')]
    [string]$RegType,

    [Parameter(Mandatory)]
    [AllowNull()]
    $Value
  )
  if ($null -eq $RegType) {
    return " "
  }
  switch -Regex ($RegType) {
    '^REG_BINARY$' {
      " /d $($(for ($i = 0; $i -lt $Value.Length; $i++) { "{0:x2}" -f $i}) -Join '') "
    }
    '^REG_MULTI_SZ$' { " /d ""$(Escape $($Value -Join "\0"))"" " }
    '^REG_(?:EXPAND_)?SZ$' { " /d ""$(Escape $Value)"" " }
    '^REG_(?:Q|D)WORD$' { " /d $Value " }
    '^REG_NONE$' { " " }
    default { throw "$RegType" }
  }
}

function DoWriteRegCommand {
  param(
    [Parameter(Mandatory)]
    [Microsoft.Win32.RegistryKey]$RegKeyObj,

    [Parameter(Mandatory)]
    [string]$Prop,

    [Parameter(Mandatory)]
    [string]$RegKey
  )
  $GetValuePropArg = if ($Prop -eq '(default)') { $null } else { $Prop }
  $Value = $RegKeyObj.GetValue($GetValuePropArg,$null,
    [Microsoft.Win32.RegistryValueOptions]::DoNotExpandEnvironmentNames)
  $RegProp = FixVParameter $Prop
  try {
    $ValueKind = $RegKeyObj.GetValueKind($GetValuePropArg).ToString()
  }
  catch {
    Write-Debug "Skipping $RegKey\$Prop. GetValueKind() failed."
    return
  }
  if ($ValueKind -eq 'Unknown') {
    Write-Debug "Skipping $RegKey\$Prop of unknown type."
    return
  }
  try {
    $RegType = GetRegType $ValueKind
  }
  catch {
    Write-Debug "Unable to determine registry type: RegKeyObj = $RegKeyObj, Prop = $Prop"
    return
  }
  if ($Value -match "(`r)?`n") {
    Write-Debug "Skipping $RegKeyObj $Prop because it contains newlines."
    return
  }
  $RegValue = ConvertValueForReg -RegType $RegType -Value $Value
  "reg add ""$(Escape $RegKey)"" $RegProp/t $RegType$RegValue/f"
}

function DoWriteRegCommands {
  param(
    [Parameter(Mandatory,HelpMessage = "Registry path.")]
    [ValidatePattern('^HK(LM|CU|CR|U|CC):')]
    [string]$Path
  )
  $Hive = switch -Wildcard ($Path) {
    'HKCC:*' { [Microsoft.Win32.Registry]::CurrentConfig }
    'HKCR:*' { [Microsoft.Win32.Registry]::ClassesRoot }
    'HKCU:*' { [Microsoft.Win32.Registry]::CurrentUser }
    'HKLM:*' { [Microsoft.Win32.Registry]::LocalMachine }
    'HKPD:*' { [Microsoft.Win32.Registry]::PerformanceData }
    'HKU:*' { [Microsoft.Win32.Registry]::Users }
    default { throw }
  }
  $PathWithoutPrefix = $Path -replace '^HK(LM|CU|CR|U|CC):',''
  $RegKey = $Path -replace ':','\' -replace '\\\\','\'
  $RegKeyObj = $Hive.OpenSubKey($PathWithoutPrefix.TrimStart('\'))
  foreach ($Prop in $(Get-Item -ErrorAction SilentlyContinue $Path | Select-Object -ExpandProperty Property)) {
    DoWriteRegCommand $RegKeyObj $Prop $RegKey
  }
}

<#
.SYNOPSIS
  Convert a registry path to a series of reg commands for copying into a script.
.DESCRIPTION
  By default only HKCU: and HKLM: are mounted in PowerShell. Others need to be mounted and must be
  under the appropriate name such as HKU for HKEY_USERS.

  Keys are skipped under these conditions:

  - Depth limit (20); this can be changed by passing -MaxDepth or -m
  - Key that cannot be read for any reason such as permissions.
  - Value contains newlines

  An example of an always skipped key under normal circumstances is HKLM\SECURITY, even if this is
  run as administrator.
.PARAMETER MaxDepth
  Depth limit.
.PARAMETER Path
  Registry path.
.PARAMETER Depth
  For internal use. Do not pass a value.
.NOTES
  WARNING: If you save an entire tree such as HKLM to a file and attempt to run said script, you
  probably will break your OS. The output of this tool is meant for getting a single command at
  time, testing it, and then using it in an appropriate script. The author will not be held
  responsible for any damages.
.EXAMPLE
  PS> Write-RegCommands 'HKCU:\Control Panel\Desktop'
  reg add ...
.EXAMPLE
  PS> prefs-export 'HKCU:\Control Panel\Desktop'
  reg add ...
#>
function Write-RegCommands {
  param(
    [Parameter(Mandatory,HelpMessage = "Registry path.")]
    [ValidatePattern('^^HK(LM|CU|CR|U|CC):')]
    [string]$Path,

    [Parameter(HelpMessage = "Depth limit.")]
    [Alias("m")]
    [int]$MaxDepth = 20,

    [Parameter(HelpMessage = "Current depth level. Used internally.")]
    [int]$Depth)
  begin {
    $SkipRe = '(^HK..:.*\\CurrentVersion\\Explorer\\.*MRU.*)|(\\\*$)|' + `
       '(.*\\Shell\\Bags\\[0-9]+\\Shell\\\{.*)'
  }
  process {
    if ($Depth -ge $MaxDepth) {
      Write-Debug "Skipping $Path due to depth limit of $MaxDepth."
      return
    }
    if ($Path -match $SkipRe) {
      Write-Debug "Skipping $Path because it matched the skip RE."
      continue
    }
    try {
      # SilentlyContinue is needed to skip HKLM\SECURITY
      $Items = Get-ChildItem -ErrorAction SilentlyContinue -Path $Path
    }
    catch {
      Write-Debug "Skipping $Path. Does the location exist?"
      return
    }
    if (!$Items) {
      $out = DoWriteRegCommands $(ReplaceFullHiveNameWithShortName $Path)
      if (!$out) {
        # Assume it is a full path to a value
        $Hive = switch -Wildcard ($Path) {
          'HKCC:*' { [Microsoft.Win32.Registry]::CurrentConfig }
          'HKCR:*' { [Microsoft.Win32.Registry]::ClassesRoot }
          'HKCU:*' { [Microsoft.Win32.Registry]::CurrentUser }
          'HKLM:*' { [Microsoft.Win32.Registry]::LocalMachine }
          'HKPD:*' { [Microsoft.Win32.Registry]::PerformanceData }
          'HKU:*' { [Microsoft.Win32.Registry]::Users }
          default { throw }
        }
        $Components = $($Path -replace '^HK(LM|CU|CR|U|CC):','').TrimStart('\').Split('\')
        $RegKeyObj = $Hive.OpenSubKey($($Components[0..($Components.Length - 2)] -join '\'))
        DoWriteRegCommand $RegKeyObj $($Path.Split('\')[-1]) $($Path -replace ':','')
      }
      else {
        Write-Output $out
      }
      return
    }
    foreach ($Item in $Items) {
      $ItemStr = $Item.ToString()
      $PathShort = ReplaceFullHiveNameWithShortName $ItemStr
      try {
        $Children = Get-ChildItem -Path $PathShort -ErrorAction SilentlyContinue
      }
      catch {
        Write-Debug "Skipping $Path because Get-ChildItem failed."
        continue
      }
      if ($Children) {
        Write-RegCommands -Path $PathShort -Depth $($Depth + 1) -MaxDepth $MaxDepth
      }
      else {
        DoWriteRegCommands -Path $PathShort
      }
    }
  }
}

<#
.SYNOPSIS
  Save registry content as reg commands to an output directory.
.DESCRIPTION
  This will save all reg commands generated by Write-RegCommands, sort them, and place them in a
  directory. It can also commit and push the changes automatically.
.PARAMETER Commit
  If set, commits to Git repository at output directory.
.PARAMETER DeployKey
  Key for pushing to Git repository. Requires -Commit.
.PARAMETER MaxDepth
  Depth limit.
.PARAMETER OutputDirectory
  Where to store the exported data.
.PARAMETER Path
  Registry path.
#>
function Save-Preferences {
  param(
    [Parameter(HelpMessage = "Commit the changes with Git.")]
    [Alias("c")]
    [switch]$Commit = $false,

    [Parameter(HelpMessage = "Key for pushing to Git repository.")]
    [Alias("K")]
    [string]$DeployKey,

    [Parameter(HelpMessage = "Where to store the exported data.")]
    [Alias("o")]
    [string]$OutputDirectory = "${env:APPDATA}\prefs-export",

    [Parameter(HelpMessage = "Depth limit.")]
    [Alias("m")]
    [int]$MaxDepth = 20,

    [Parameter(HelpMessage = "Registry path.")]
    [ValidatePattern('^^HK(LM|CU|CR|U|CC):')]
    [string]$Path = 'HKCU:'
  )
  if ($DeployKey) {
    $DeployKey = Resolve-Path -Path $DeployKey -ErrorAction SilentlyContinue
  }
  New-Item -Force -ItemType directory -Path "$OutputDirectory" | Out-Null
  Write-RegCommands -MaxDepth $MaxDepth -Path $Path | `
     Sort-Object -CaseSensitive -Unique > "$OutputDirectory\exec-reg.bat"
  $Git = (Get-Command git).Path
  if ($Commit -and $Git) {
    if (-not (Test-Path -PathType Container -Path ".git")) {
      Write-Debug "Init"
      $OriginalLocation = Get-Location
      Set-Location $OutputDirectory
      git init
      Set-Location -Path $OriginalLocation
    }
    Write-Debug "Committing changes"
    git "--git-dir=$OutputDirectory\.git" "--work-tree=$OutputDirectory" add .
    git "--git-dir=$OutputDirectory\.git" "--work-tree=$OutputDirectory" commit --no-gpg-sign `
       --quiet --no-verify "--author=winprefs <winprefs@tat.sh>" `
       -m "Automatic commit @ $(Get-Date -UFormat %c)"
    if (Test-Path -PathType Leaf -Path $DeployKey) {
      git "--git-dir=$OutputDirectory\.git" "--work-tree=$OutputDirectory" config core.sshCommand `
         "ssh -i ${DeployKey} -F nul -o UserKnownHostsFile=nul -o StrictHostKeyChecking=no"
      git "--git-dir=$OutputDirectory\.git" "--work-tree=$OutputDirectory" push -u --porcelain `
         --no-signed origin origin $(git branch --show-current)
    }
  }
}

function GetSafePathName {
  param(
    [Parameter(Mandatory)]
    [string]$Path
  )
  $Path -replace ':','-' -replace '\\','-' -replace '-+$',''
}

<#
.SYNOPSIS
  Create and register a new scheduled task to save preferences.
.DESCRIPTION
  The name of the task will be of format 'SavePreferences-<FIXED PATH>'. The default task is named
  SavePreferences-HKCU. A task to save 'HKCU:\Control Panel' would be named 'SavePreferences
  HKCU-Control Panel'. The task is run every 12 hours starting on the next day at midnight (00:00).

  Execution-Policy has to be modified for this to work. It must be at least Bypass.
.PARAMETER DeployKey
  Key for pushing to Git repository.
.PARAMETER MaxDepth
  Depth limit.
.PARAMETER OutputDirectory
  Where to store the exported data. This can be an unexpanded string containing variable references.
.PARAMETER Path
  Registry path.
.EXAMPLE
  PS> Register-SavePreferencesScheduledTask
#>
function Register-SavePreferencesScheduledTask {
  param(
    [Parameter(HelpMessage = "Key for pushing to Git repository.")]
    [Alias("K")]
    [string]$DeployKey,

    [Parameter(HelpMessage = "Depth limit.")]
    [Alias("m")]
    [int]$MaxDepth = 20,

    [Parameter(
      HelpMessage = "Where to store the exported data."
    )]
    [Alias("o")]
    [string]$OutputDirectory = '${env:APPDATA}\prefs-export',

    [Parameter(HelpMessage = "Registry path.")]
    [ValidatePattern('^^HK(LM|CU|CR|U|CC):')]
    [string]$Path = 'HKCU:'
  )
  $TasksDir = "${env:APPDATA}\WinPrefs\tasks"
  New-Item -Force -ItemType directory -Path $TasksDir | Out-Null
  $SafePathName = GetSafePathName $Path
  $TaskFile = "$TasksDir\export-$(($SafePathName).ToLower()).ps1"
  if (Test-Path -PathType leaf $TaskFile) {
    Write-Warning 'Task file for this path already exists. Overwriting.'
  }
  $DeployKeyArg = if ($DeployKey -and (Test-Path -PathType leaf $DeployKey)) {
    " -DeployKey ""$DeployKey"""
  }
  else { '' }
  Write-Output "Import-Module WinPrefs" > $TaskFile
  Write-Output "Save-Preferences -Commit$DeployKeyArg -MaxDepth $MaxDepth ``" >> $TaskFile
  Write-Output "  -OutputDirectory ""$OutputDirectory"" -Path ""$Path""" >> $TaskFile
  $TaskName = "SavePreferences-$SafePathName"
  $TaskPath = "\tat.sh\WinPrefs\"
  if (Get-ScheduledTaskInfo -TaskName $TaskName -TaskPath $TaskPath) {
    # Only updating the script is necessary.
    Write-Output -NoEnumerate `
       "Task already exists. If you want to restore to the default task settings, you must delete the
existing task '$TaskPath\$TaskName' in Task Scheduler. You can also run
Unregister-SavePreferencesScheduledTask with the same -Path argument."
    return
  }
  $Action = New-ScheduledTaskAction `
     -Argument "-ExecutionPolicy Bypass -NoLogo -NonInteractive -NoProfileLoadTime -WindowStyle Hidden -File $TaskFile" `
     -Execute 'pwsh.exe' `
     -WorkingDirectory $HOME
  $Trigger = New-ScheduledTaskTrigger `
     -At ([datetime]::Today.AddDays(1)) `
     -Once `
     -RandomDelay (New-TimeSpan -Minutes 30) `
     -RepetitionInterval (New-TimeSpan -Hours 12)
  $Settings = New-ScheduledTaskSettingsSet `
     -MultipleInstances IgnoreNew `
     -ExecutionTimeLimit (New-TimeSpan -Hours 2) `
     -StartWhenAvailable
  Register-ScheduledTask `
     -Action $Action `
     -Description "Run SavePreferences every 12 hours (path $Path)." `
     -Force `
     -Settings $Settings `
     -TaskName "SavePreferences-$SafePathName" `
     -TaskPath $TaskPath `
     -Trigger $Trigger
}

<#
.SYNOPSIS
  Remove an existing scheduled task for a registry path.
.DESCRIPTION
  This also cleans up tat.sh/WinPrefs scheduled task directory trees and WinPrefs-related
  directories in APPDATA.
.PARAMETER Path
  Registry path.
.EXAMPLE
  PS> Unregister-SavePreferencesScheduledTask
#>
function Unregister-SavePreferencesScheduledTask {
  param(
    [Parameter(HelpMessage = "Registry path.")]
    [ValidatePattern('^^HK(LM|CU|CR|U|CC):')]
    [string]$Path = 'HKCU:'
  )
  $SafePathName = GetSafePathName $Path
  $TaskName = "SavePreferences-$SafePathName"
  $TasksDir = "${env:APPDATA}\WinPrefs\tasks"
  $TaskFile = "$TasksDir\export-$(($SafePathName).ToLower()).ps1"
  $TaskPath = "\tat.sh\WinPrefs\"
  if (Get-ScheduledTaskInfo -TaskName $TaskName -TaskPath $TaskPath) {
    Disable-ScheduledTask -TaskName $TaskName -TaskPath $TaskPath
    Stop-ScheduledTask -TaskName $TaskName -TaskPath $TaskPath
    Unregister-ScheduledTask -TaskName $TaskName -TaskPath $TaskPath -Confirm:$false
    Remove-Item -Force -Path $TaskFile -Confirm:$false
  }
  # If everything is empty under tat.sh\WinPrefs delete the directories.
  if (-not (Get-ScheduledTask | Where-Object { $_.TaskPath -eq '\tat.sh\WinPrefs\' })) {
    $ScheduleObject = New-Object -ComObject Schedule.Service
    $ScheduleObject.connect()
    $ScheduleObject.GetFolder('\').DeleteFolder('tat.sh\WinPrefs',$null)
    if (-not (Get-ScheduledTask | Where-Object { $_.TaskPath -eq '\tat.sh\' })) {
      # Again for tat.sh
      $ScheduleObject.GetFolder('\').DeleteFolder('tat.sh',$null)
    }
  }
  if (((Get-ChildItem -Path $TasksDir -Force) | Measure-Object).Count -eq 0) {
    Remove-Item -Force -Confirm:$false -Path $TasksDir
    $PrefsDir = "${env:APPDATA}\WinPrefs"
    if (((Get-ChildItem -Path $PrefsDir -Force) | Measure-Object).Count -eq 0) {
      Remove-Item -Force -Confirm:$false -Path $PrefsDir
    }
  }
}

Set-Alias -Name path2reg -Value Write-RegCommands
Set-Alias -Name prefs-export -Value Save-Preferences
Set-Alias -Name winprefs-install-job -Value Register-SavePreferencesScheduledTask
Set-Alias -Name winprefs-uninstall-job -Value Unregister-SavePreferencesScheduledTask
Export-ModuleMember -Alias path2reg,prefs-export,winprefs-install-job,winprefs-uninstall-job `
   -Function Register-SavePreferencesScheduledTask,Save-Preferences,`
   Unregister-SavePreferencesScheduledTask,Write-RegCommands
