# winprefs

[![QA](https://github.com/Tatsh/winprefs/actions/workflows/qa.yml/badge.svg)](https://github.com/Tatsh/winprefs/actions/workflows/qa.yml)
[![PowerShell Gallery Version (including pre-releases)](https://img.shields.io/powershellgallery/v/WinPrefs)](https://www.powershellgallery.com/packages/WinPrefs)
[![PowerShell Gallery Platform Support](https://img.shields.io/powershellgallery/p/WinPrefs?label=powershell+platforms+supported)](https://www.powershellgallery.com/packages/WinPrefs)
![GitHub tag (with filter)](https://img.shields.io/github/v/tag/Tatsh/winprefs)
![GitHub](https://img.shields.io/github/license/Tatsh/winprefs)
![GitHub commits since latest release (by SemVer including pre-releases)](https://img.shields.io/github/commits-since/Tatsh/winprefs/v0.2.0/master)

macOS users: please see [macprefs](https://github.com/Tatsh/macprefs) for an equivalent tool.

This package dumps a registry path to equivalent `reg` commands. It contains two commands.
`Save-Preferences` creates a batch script composed of `reg` commands for copying into a script.
`Write-RegCommands` is effectively the same but requires a path argument and only writes to standard
output.

By default both commands default to a maximum depth of 20. `Save-Preferences` defaults to path
`HKCU:`.

Note that by default only `HKCU:` and `HKLM:` are mounted in PowerShell. Others need to be mounted
and must be under the appropriate name such as `HKU` for `HKEY_USERS`.

Keys/values are skipped under these conditions:

- Depth limit (default: 20); this can be changed by passing `-MaxDepth LIMIT` or `-m LIMIT`
- Key that cannot be read for any reason such as permissions
- Value contains newlines
- Value has type `REG_UNKNOWN`

An example of an always skipped key under normal circumstances is `HKLM\SECURITY`, even if this is
run as administrator.

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
Save-Preferences
# or the alias:
prefs-export
```

`Save-Preferences` generates an `exec-reg.bat` file and saves to `${env:APPDATA}\prefs-export` by
default. This can be changed by passing `-OutputDirectory DIR` (or `-o DIR`).

It accepts switch `-Commit`/`-c` to initialise and commit to a Git repository in the output
directory. It also accepts a `-DeployKey PATH` parameter and will push if this is specified. Any
other Git management such as the branch name, etc must be managed in the output directory manually.

```powershell
Write-RegCommands HKCU:
# or the alias
path2reg HKCU:
```

`Write-RegCommands` prints out equivalent reg commands to reproduce the keys/values at the given
path. `reg` command output is escaped for Batch file only. No variables will be present in the
output. If you want to use a `reg` command in PowerShell you need to replace `%%` with `%`.

## Automated Usage

You can create a scheduled task that will run every 12 hours to backup a registry path.
`Register-SavePreferencesScheduledTask` (`winprefs-install-job`) can be called multiple times with
different `-Path` arguments. Like `Save-Preferences` it can automatically commit to a repository and
push. Because it uses a PowerShell script, `Execution-Policy` must be changed to `Bypass`:

```powershell
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope CurrentUser
```

Calling `Register-SavePreferencesScheduledTask` multiple times with the same `-Path` argument will
not break anything. If a task with the same name already exists, it must be unregistered before its
replacement can be made.

Any task can be uninstalled with `Unregister-SavePreferencesScheduledTask` (`winprefs-uninstall-job`)
with the same `-Path` argument.

### Examples

#### Save `HKEY_LOCAL_MACHINE\Control Panel` with a depth of 1

```powershell
prefs-export -o . -m 1 -Path 'HKCU:\Control Panel\Desktop'
cat exec-reg.bat
```

Output:

```batch
reg add "HKCU\Control Panel\Desktop\Colors" /v "ActiveBorder" /t REG_SZ /d "212 208 200" /f
reg add "HKCU\Control Panel\Desktop\Colors" /v "ActiveTitle" /t REG_SZ /d "10 36 106" /f
REM ...
```

### Export, commit with Git and push with a key

#### Dump the local environment variables

```powershell
Write-RegCommands HKCU:\Environment
```

Output:

```batch
reg add "HKCU\Environment" /v "OneDrive" /t REG_EXPAND_SZ /d "%%USERPROFILE%%\OneDrive" /f
reg add "HKCU\Environment" /v "Path" /t REG_EXPAND_SZ /d "%%USERPROFILE%%\AppData\Local\Microsoft\WindowsApps;C:\msys64\mingw64\bin;C:\tools\Cmder;" /f
REM ...
```

#### Dump a single value

```powershell
Write-RegCommands HKCU:\Environment\OneDrive
```

Output:

```batch
reg add "HKCU\Environment\OneDrive" /v "OneDrive" /t REG_EXPAND_SZ /d "%%USERPROFILE%%\OneDrive" /f
```

#### Dump a binary value

```powershell
Write-RegCommands 'HKCU:\Control Panel\Desktop\WindowMetrics\StatusFont'
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
