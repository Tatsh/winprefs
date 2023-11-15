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
  $Value -replace '"', '""' -replace '%', '%%'
}

function GetSafePathName {
  param(
    [Parameter(Mandatory)]
    [string]$Path
  )
  $Path -replace ':', '-' -replace '\\', '-' -replace '-+$', ''
}
