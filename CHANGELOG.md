# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project
adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.3.1] - 2023-11-19

### Fixed

- Paths with unsupported value types now works with the C, C# and `reg add` output formats

## [0.3.0] - 2023-11-19

### Added

- Manual page

## [0.3.0-b0] - 2023-11-18

### Added

- New output formats: PowerShell, C, and C#

### Changed

- Mostly rewrote in C
- Native code: 100% code coverage from tests
- Re-wrote PowerShell part in C#
- Source reorganisation
- The PowerShell (from its C# DLL) calls the native DLL. This is significantly faster than the
  100% PowerShell prior version.
- Update documentation

## [0.2.1] - 2023-10-08

### Fixed

- Use `pwsh.exe` not `powershell.exe` for the scheduled task.
- Fixed invoking the scheduled task.

### Changed

- Try to hide the window of the scheduled task. Affected by
  [PowerShell #3028](https://github.com/PowerShell/PowerShell/issues/3028).

## [0.2.0] - 2023-10-08

### Added

- Add `Register-SavePreferencesScheduledTask` and `Unregister-SavePreferencesScheduledTask` to
  setup an automatic backup job. Can have multiple jobs with different paths.

## [0.1.0] - 2023-10-07

### Added

- `Save-Preferences` and made `prefs-export` match the interface used in
  [macprefs](https://github.com/Tatsh/macprefs).
  - Can now commit with Git and push
- Badges to readme

### Changed

- Set default maximum depth to 20.
- Update documentation
- Format code

[0.2.1]: https://github.com/Tatsh/winprefs/compare/v0.2.1...HEAD
[0.2.0]: https://github.com/Tatsh/winprefs/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/Tatsh/winprefs/compare/v0.0.2...v0.1.0
