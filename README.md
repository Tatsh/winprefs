# winprefs

[![QA](https://github.com/Tatsh/winprefs/actions/workflows/qa.yml/badge.svg)](https://github.com/Tatsh/winprefs/actions/workflows/qa.yml)
[![PowerShell Gallery Version (including pre-releases)](https://img.shields.io/powershellgallery/v/WinPrefs)](https://www.powershellgallery.com/packages/WinPrefs)
[![PowerShell Gallery Platform Support](https://img.shields.io/powershellgallery/p/WinPrefs?label=powershell+platforms+supported)](https://www.powershellgallery.com/packages/WinPrefs)

macOS users: please see [macprefs](https://github.com/Tatsh/macprefs) for an equivalent tool.

This tool converts a registry path to a series of reg commands for copying into a script. By
default only `HKCU:` and `HKLM:` are mounted in PowerShell. Others need to be mounted and must be
under the appropriate name such as `HKU` for `HKEY_USERS`.

Keys/values are skipped under these conditions:

- Recursion limit (100)
- Value contains newlines
- Key that cannot be read for any reason such as permissions
- Values of type `REG_UNKNOWN`

An example of an always skipped key under normal circumstances is `HKLM\SECURITY`, even if this is
run as adminstrator.

WARNING: If you save an entire tree such as `HKLM:` to a file and attempt to run said script, you
probably will break your OS. The output of this tool is meant for getting a single command at a
time, testing it, and then using it in an appropriate script. The author will not be held
responsible for any damages.

## Installation

```powershell
Install-Module -Name WinPrefs
```

## Usage

```powershell
Write-RegCommands HKCU:
# or the alias:
prefs-export HKCU:
```

The output is escaped for Batch file only. No variables will be present in the output.

If you want to use a `reg` command in PowerShell you need to replace `%%` with `%`.

### Examples

#### Dump the local environment variables

```powershell
prefs-export HKCU:\Environment
```

Output:

```batch
reg add "HKCU\Environment" /v "OneDrive" /t REG_EXPAND_SZ /d "%%USERPROFILE%%\OneDrive" /f
reg add "HKCU\Environment" /v "Path" /t REG_EXPAND_SZ /d "%%USERPROFILE%%\AppData\Local\Microsoft\WindowsApps;C:\msys64\mingw64\bin;C:\tools\Cmder;" /f
reg add "HKCU\Environment" /v "TEMP" /t REG_EXPAND_SZ /d "%%USERPROFILE%%\AppData\Local\Temp" /f
reg add "HKCU\Environment" /v "TMP" /t REG_EXPAND_SZ /d "%%USERPROFILE%%\AppData\Local\Temp" /f
reg add "HKCU\Environment" /v "ChocolateyToolsLocation" /t REG_SZ /d "C:\tools" /f
```

#### Dump a single value

```powershell
prefs-export HKCU:\Environment\OneDrive
```

Output:

```batch
reg add "HKCU\Environment\OneDrive" /v "OneDrive" /t REG_EXPAND_SZ /d "%%USERPROFILE%%\tatsh\OneDrive" /f
```

#### Dump a binary value

```powershell
prefs-export 'HKCU:\Control Panel\Desktop\WindowMetrics\StatusFont'
```

Output:

```batch
reg add "HKCU\Control Panel\Desktop\WindowMetrics\StatusFont" /v "StatusFont" /t REG_BINARY /d 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b /f
```

## Development

Requirements:

- Windows
- Yarn

### Tasks

- `yarn format`: to format the project's files.
- `yarn qa`: Perform a QA check.
