function ReplaceFullHiveNameWithShortName {
  param(
    [Parameter(Mandatory, HelpMessage = "Registry path with long name and no drive syntax.")]
    [string]$Path)
  $_unused, $HkeyParts = $Path.ToUpper().Split('\')[0].Split('_')
  $Path -replace '^HKEY_[^\\]+\\', "HK$($(foreach ($Item in $HkeyParts) { $Item[0] }) -Join ''):"
}

function GetFullHiveName {
  param(
    [Parameter(Mandatory, HelpMessage = "Registry path.")]
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
