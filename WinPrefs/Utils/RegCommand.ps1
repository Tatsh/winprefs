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
  param(
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
  $PathWithoutPrefix = $Path -replace '^HK(LM|CU|CR|U|CC):', ''
  $RegKey = $Path -replace ':', '\' -replace '\\\\', '\'
  $RegKeyObj = $Hive.OpenSubKey($PathWithoutPrefix.TrimStart('\'))
  foreach ($Prop in $(Get-Item -ErrorAction SilentlyContinue $Path | Select-Object -ExpandProperty Property)) {
    DoWriteRegCommand $RegKeyObj $Prop $RegKey
  }
}
