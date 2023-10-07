function ReplaceFullHiveNameWithShortName {
  param (
    [Parameter(Mandatory, HelpMessage = "Registry path with long name and no drive syntax.")]
    [string] $Path)
  $_unused, $HkeyParts = $Path.ToUpper().Split('\')[0].Split('_')
  $Path -Replace '^HKEY_[^\\]+\\', "HK$($(foreach ($Item in $HkeyParts) { $Item[0] }) -Join ''):"
}

function GetFullHiveName {
  param (
    [Parameter(Mandatory, HelpMessage = "Registry path.")]
    [ValidatePattern('^HK(LM|CU|CR|U|CC|PD):')]
    [string] $Path
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

function GetRegType() {
  param (
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
  param (
    [Parameter(Mandatory)]
    [string] $Prop
  )
  if ($Prop -eq '(default)') {
    '/ve '
  }
  else {
    "/v ""$(Escape $Prop)"" "
  }
}

function Escape {
  param (
    [Parameter(Mandatory)]
    [AllowNull()]
    [AllowEmptyString()]
    [string]$Value
  )
  if ($null -eq $Value) {
    return ""
  }
  $Value -Replace '"', '""' -Replace '%', '%%' # -Replace "(`r)?`n", "!LF!"
}

function ConvertValueForReg {
  param (
    [Parameter(Mandatory)]
    [ValidatePattern('^REG_(BINARY|(?:Q|D)WORD|(?:(?:EXPAND|MULTI)_)?SZ|NONE)')]
    [string] $RegType,

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
    '^REG_MULTI_SZ$' { " /d ""$(Escape $($Value -Join "`0"))"" " }
    '^REG_(?:EXPAND_)?SZ$' { " /d ""$(Escape $Value)"" " }
    '^REG_(?:Q|D)WORD$' { " /d $Value " }
    '^REG_NONE$' { " " }
    default { throw "$RegType" }
  }
}

function DoWriteRegCommand {
  param (
    [Parameter(Mandatory)]
    [Microsoft.Win32.RegistryKey]$RegKeyObj,

    [Parameter(Mandatory)]
    [string]$Prop,

    [Parameter(Mandatory)]
    [string]$RegKey
  )
  $GetValuePropArg = if ($Prop -eq '(default)') { $null } else { $Prop }
  $Value = $RegKeyObj.GetValue($GetValuePropArg, $null,
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
  param (
    [Parameter(Mandatory, HelpMessage = "Registry path.")]
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
  $PathWithoutPrefix = $Path -Replace '^HK(LM|CU|CR|U|CC):', ''
  $RegKey = $Path -Replace ':', '\' -Replace '\\\\', '\'
  $RegKeyObj = $Hive.OpenSubKey($PathWithoutPrefix.TrimStart('\'))
  foreach ($Prop in $(Get-Item -ErrorAction SilentlyContinue $Path | Select-Object -ExpandProperty Property)) {
    DoWriteRegCommand $RegKeyObj $Prop $RegKey
  }
}

$LIMIT = 100
$SKIP_RE = '(^HK..:.*\\CurrentVersion\\Explorer\\.*MRU.*)|(\\\*$)|(.*\\Shell\\Bags\\[0-9]+\\Shell\\\{.*)'

function DoWriteRegCommandsRecursive {
  param (
    [Parameter(Mandatory, HelpMessage = "Registry path.")]
    [ValidatePattern('^^HK(LM|CU|CR|U|CC):')]
    [string]$Path,

    [Parameter(HelpMessage = "Current depth level. Used internally.")]
    [int]$Depth)
  if ($Depth -ge $LIMIT) {
    Write-Debug "Skipping $Path due to depth limit."
    return
  }
  if ($Path -match $SKIP_RE) {
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
      $Components = $($Path -Replace '^HK(LM|CU|CR|U|CC):', '').TrimStart('\').Split('\')
      $RegKeyObj = $Hive.OpenSubKey($($Components[0..($Components.Length - 2)] -Join '\'))
      DoWriteRegCommand $RegKeyObj $($Path.Split('\')[-1]) $($Path -Replace ':', '')
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
      DoWriteRegCommandsRecursive -Path $PathShort -Depth $($Depth + 1)
    }
    else {
      DoWriteRegCommands -Path $PathShort
    }
  }
}

<#
  .SYNOPSIS
  Convert a registry path to a series of reg commands for copying into a script. By default only
  HKCU: and HKLM: are mounted in PowerShell. Others need to be mounted and must be under the
  appropriate name such as HKU for HKEY_USERS.

  Keys are skipped under these conditions:
  - Recursion limit (100)
  - Value contains newlines
  - Key that cannot be read for any reason such as permissions.

  An example of an always skipped key under normal circumstances is HKLM\SECURITY, even if this is
  run as administrator.

  WARNING: If you save an entire tree such as HKLM to a file and attempt to run said script, you
  probably will break your OS. The output of this tool is meant for getting a single command at
  time, testing it, and then using it in an appropriate script. The author will not be held
  responsible for any damages.

  .EXAMPLE
    # Dump reg commands for desktop settings.
    Write-RegCommands 'HKCU:\Control Panel\Desktop'

    # Use the alias.
    prefs-export 'HKCU:\Control Panel\Desktop'

    # Dump the entire HKLM (note skipped keys above) and save to a script
    prefs-export HKLM: > hklm.bat
#>
function Write-RegCommands {
  param (
    [Parameter(Mandatory, HelpMessage = "Registry path.")]
    [ValidatePattern('^^HK(LM|CU|CR|U|CC):')]
    [string]$Path
  )
  # Write-Output "setlocal EnableDelayedExpansion"
  # Write-Output "(set LF=^"
  # Write-Output "%=EMPTY=%"
  # Write-Output ")"
  DoWriteRegCommandsRecursive $Path
}
Set-Alias -Name prefs-export -Value Write-RegCommands
Export-ModuleMember -Function Write-RegCommands -Alias prefs-export
