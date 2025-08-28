# winprefs

[![PowerShell Gallery Version (including pre-releases)](https://img.shields.io/powershellgallery/v/WinPrefs)](https://www.powershellgallery.com/packages/WinPrefs)
[![PowerShell Gallery Platform Support](https://img.shields.io/powershellgallery/p/WinPrefs?label=powershell+platforms+supported)](https://www.powershellgallery.com/packages/WinPrefs)
[![PowerShell Gallery Downloads](https://img.shields.io/powershellgallery/dt/WinPrefs)](https://www.powershellgallery.com/packages/WinPrefs)
[![GitHub tag (with filter)](https://img.shields.io/github/v/tag/Tatsh/winprefs)](https://github.com/Tatsh/winprefs/tags)
[![License](https://img.shields.io/github/license/Tatsh/winprefs)](https://github.com/Tatsh/winprefs/blob/master/LICENSE.txt)
[![GitHub commits since latest release (by SemVer including pre-releases)](https://img.shields.io/github/commits-since/Tatsh/winprefs/v0.3.2/master)](https://github.com/Tatsh/winprefs/compare/v0.3.2...master)
[![Build](https://github.com/Tatsh/winprefs/actions/workflows/cmake.yml/badge.svg)](https://github.com/Tatsh/winprefs/actions/workflows/cmake.yml)
[![CodeQL](https://github.com/Tatsh/winprefs/actions/workflows/codeql.yml/badge.svg)](https://github.com/Tatsh/winprefs/actions/workflows/codeql.yml)
[![QA](https://github.com/Tatsh/winprefs/actions/workflows/qa.yml/badge.svg)](https://github.com/Tatsh/winprefs/actions/workflows/qa.yml)
[![Tests](https://github.com/Tatsh/winprefs/actions/workflows/tests.yml/badge.svg)](https://github.com/Tatsh/winprefs/actions/workflows/tests.yml)
[![Coverage Status](https://coveralls.io/repos/github/Tatsh/winprefs/badge.svg?branch=master)](https://coveralls.io/github/Tatsh/winprefs?branch=master)
[![GitHub Pages](https://github.com/Tatsh/winprefs/actions/workflows/pages.yml/badge.svg)](https://tatsh.github.io/winprefs/)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit&logoColor=white)](https://github.com/pre-commit/pre-commit)
[![Stargazers](https://img.shields.io/github/stars/Tatsh/winprefs?logo=github&style=flat)](https://github.com/Tatsh/winprefs/stargazers)

[![@Tatsh](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fpublic.api.bsky.app%2Fxrpc%2Fapp.bsky.actor.getProfile%2F%3Factor%3Ddid%3Aplc%3Auq42idtvuccnmtl57nsucz72%26query%3D%24.followersCount%26style%3Dsocial%26logo%3Dbluesky%26label%3DFollow%2520%40Tatsh&query=%24.followersCount&style=social&logo=bluesky&label=Follow%20%40Tatsh)](https://bsky.app/profile/Tatsh.bsky.social)
[![Mastodon Follow](https://img.shields.io/mastodon/follow/109370961877277568?domain=hostux.social&style=social)](https://hostux.social/@Tatsh)

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

`Save-Preferences` generates an `exec-reg.bat` (default name) file and saves to
`${env:APPDATA}\prefs-export` by default. This can be changed by passing `-OutputDirectory DIR`
(or `-o DIR`). The file name can be changed by passing `-OutputFile FILE` (or `-f FILE`).

The format can be changed with the `-Format` argument. Accepted format strings:

-

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
push.

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

#### Dump WindowMetrics as C code

```powershell
Write-RegCommands HKCU:\Environment -Format c
```

Output:

```c
data = { 0xee, 0xff, 0xff, 0xff, /* ... */}; RegSetKeyValue(HKEY_CURRENT_USER, TEXT("Control Panel\\Desktop\\WindowMetrics"), TEXT("CaptionFont"), REG_BINARY, (LPCVOID)&data, 92);
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

## Native version

The PowerShell module makes use of the native code with `DllImport` to get a significant speed
increase in making registry queries and performing I/O. A native binary `winprefs.exe` can also be
built that can be used without needing PowerShell installed and it should work with XP and newer. If
built as a a 32-bit binary, the native binary should be compatible with Windows 2000 and newer. It
even works with Wine.

Usage is similar to the PowerShell version:

```plain
Usage: winprefs.exe [OPTION...] [REG_PATH]

If a path to a value name is specified, the output directory argument is ignored and the line is
printed to standard output.

Options:
  -F, --format=FORMAT Format to output. Options: c, cs, c#, ps, ps1, powershell, reg. Default: reg.
  -K, --deploy-key    Deploy key for committing.
  -c, --commit        Commit changes.
  -d, --debug         Enable debug logging.
  -f, --output-file   Output filename.
  -m, --max-depth=INT Set maximum depth.
  -o, --output-dir    Output directory.
  -h, --help          Display this help and exit.
```

Note the equivalent format names (case-insensitive):

- C#: `cs`, `c#`
- PowerShell: `ps`, `ps1`, `powershell`

## Development

Requirements:

- Windows
- Yarn

### Building

#### Visual Studio

On Windows with Visual Studio installed:

```shell
mkdir build
cd build
cmake -Wno-dev ..
cmake -G 'Visual Studio 17 2022' --build . -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

Using the VS developer environment shell is not required but it may help.

After building `winprefs.exe` will be in `build\native\Release\winprefs.exe`.

#### MinGW

Note: The PowerShell module (C# code) is only buildable with Visual Studio 2022.

```shell
mkdir build
cd build
cmake -G Ninja -Wno-dev ..
cmake --build . --config Release --verbose
```

After building `winprefs.exe` will be in `build/native/winprefs.exe`.

#### Cross-compiling

```shell
mkdir build
cd build
cmake -G Ninja -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON -DCMAKE_C_COMPILER=/usr/lib/mingw64-toolchain/bin/x86_64-w64-mingw32-gcc -DCMAKE_SYSTEM_NAME=Windows -Wno-dev ..
cmake --build . --config Release --verbose
```

You may need to adjust paths above.

After building `winprefs.exe` will be in `build/native/winprefs.exe`.

### Tasks

- `yarn format`: to format the project's files.
- `yarn qa`: Perform a QA check.
