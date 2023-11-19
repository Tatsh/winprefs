@{
  command     = 'Write-RegCommands'
  synopsis    = 'Convert a registry path to a series of reg commands for copying into a script.'
  description = 'By default only HKCU: and HKLM: are mounted in PowerShell. Others need to be mounted and must be under the appropriate name such as HKU for HKEY_USERS.

  Keys are skipped under these conditions:

  - Depth limit (20); this can be changed by passing -MaxDepth or -m
  - Key that cannot be read for any reason such as permissions.
  - Value contains newlines

  An example of an always skipped key under normal circumstances is HKLM\SECURITY, even if this is run as administrator.'
  parameters  = @{
    Format   = 'Output format. Acceptable values: c, c#, ps, ps1, powershell.'
    MaxDepth = 'Depth limit.'
    Path     = 'Registry path.'
  }
  notes       = 'WARNING: If you save an entire tree such as HKLM to a file and attempt to run said script, you probably will break your OS. The output of this tool is meant for getting a single command at time, testing it, and then using it in an appropriate script. The author will not be held responsible for any damages.'
  examples    = @(
    @{
      code = {
        PS> Write-RegCommands 'HKCU:\Control Panel\Desktop'
      }
    }
  )
}
