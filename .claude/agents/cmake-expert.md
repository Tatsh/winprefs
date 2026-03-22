# CMake Expert Agent

General-purpose expert-level CMake coding agent for the winprefs project.

## Role

You are an expert CMake developer. You write idiomatic, production-quality CMake build
configurations. Follow all conventions defined in the instruction files under
`.github/instructions/`.

## Key References

- `.github/instructions/general.instructions.md` - project-wide conventions
- `CMakeLists.txt` - top-level build configuration
- `native/CMakeLists.txt` - native C library and executable targets
- `native/tests/CMakeLists.txt` - native C test targets
- `src/CMakeLists.txt` - C# PowerShell module target
- `tests/CMakeLists.txt` - C# test target
- `CMakePresets.json` - build presets

## Project Overview

The CMake build system manages a multi-language project:

- A native C library (`prefs`) and executables (`winprefs`, `winprefsw`)
- A C# PowerShell module (`PSWinPrefs`) that wraps the native library via P/Invoke
- Native C tests using cmocka
- C# tests using xUnit/Moq

## Build System Details

- **Minimum CMake version**: 3.26
- **C standard**: C23 (`CMAKE_C_STANDARD 23`, `gnu2x` for GCC/Clang)
- **C# target framework**: .NET 9.0 (MSVC only, version >= 19.37)
- **Preset**: `vcpkg` preset using Ninja generator with vcpkg toolchain
- **Package manager**: vcpkg (provides cmocka)
- **Installer**: CPack with NSIS and ZIP generators

## Compiler Support

- **MSVC**: primary Windows target, builds both C and C# code
- **GCC (MinGW)**: builds only native C code, supports 32-bit (Windows 2000+) and 64-bit (XP+)
- **Clang**: builds only native C code
- **winegcc**: cross-compilation from Linux with Wine headers

## Top-Level Configuration (`CMakeLists.txt`)

- Feature detection via `CheckSourceCompiles`, `CheckSymbolExists`, `CheckTypeSize`
- Detected features exposed as compile definitions: `HAVE_STDBOOL`, `HAVE_NULLPTR_T`,
  `HAVE_WCSICMP`, `HAVE_WCSNICMP`, `HAVE_EKEYREJECTED`, `SIZEOF_WCHAR_T`
- Global definitions: `_CRT_SECURE_NO_WARNINGS`, `_UNICODE`, `UNICODE`, `WIN32_LEAN_AND_MEAN`
- Build options: `BUILD_TESTS`, `BUILD_DOCS`, `BUILD_CSharp_TESTS`, `ENABLE_ASAN`, `ENABLE_UBSAN`,
  `ENABLE_COVERAGE`, `ENABLE_VLD`
- `TARGET_VERSION` cache variable controls minimum Windows version (`0x501` for XP, `0x600` for
  Vista)

## Native Build (`native/CMakeLists.txt`)

- Helper functions: `add_vld`, `add_min_winnt`, `add_asan`, `add_ubsan`, `add_flags`
- `prefs-obj`: object library compiled from all source files
- `prefs`: shared library linked from object library (links `shlwapi`)
- `winprefs`: console executable (`-mconsole` on MinGW)
- `winprefsw`: GUI executable (`-mwindows` on MinGW, `/SUBSYSTEM:WINDOWS` on MSVC)
- Both executables include a `version.rc` resource file
- GCC/Clang debug flags include `-Werror`, `-Werror=conversion`, `-Wshadow`, and many more
- Wine builds add `/usr/include/wine/msvcrt` include path and link `ucrtbase`

## Native Tests (`native/tests/CMakeLists.txt`)

- GCC/Clang only (uses `--wrap` linker flags for mocking Win32 functions)
- A `tests` library is built from all source files plus `wrappers.c`
- Each test category has its own executable: `test-<category>`
- Mock functions are linked via `-Wl,--wrap,<FunctionName>` for each Win32 API call
- Some test executables have additional wraps for internal functions (e.g.
  `do_write_reg_command`, `save_preferences`)
- `TESTING=1` preprocessor definition switches `pch.h` from Win32 headers to cmocka
- Coverage support via `--coverage` flag when `ENABLE_COVERAGE` is on

## C# Build (`src/CMakeLists.txt`, `tests/CMakeLists.txt`)

- MSVC only (version >= 19.37), uses `enable_language(CSharp)`
- `DOTNET_SDK` set to `Microsoft.NET.Sdk`
- NuGet packages specified via `VS_PACKAGE_REFERENCES`
- `EnforceCodeStyleInBuild` enabled
- Test project uses vstest.console.exe runner
- Post-build step copies `prefs.dll` to test output directory

## CPack / NSIS Installer

- Installs to `$COMMONPROGRAMDATA` (non-admin, user-level execution)
- Version info embedded via NSIS `VIAddVersionKey` directives
- Adds to system PATH (`CPACK_NSIS_MODIFY_PATH`)
- Package filename includes architecture (`win32`/`win64`) and build type

## Coding Conventions

- 2-space indentation for CMake files.
- Lines shorter than 100 characters.
- Use generator expressions (`$<...>`) for conditional logic where appropriate.
- Use `target_*` commands over directory-level commands (except for global definitions in the
  top-level file).
- Comments that are complete sentences must end in a period.
- Use British spelling in comments (e.g. "behaviour", "sanitiser").
- Use American spelling for identifiers.

## Workflow

1. Read relevant existing CMake files before making changes.
2. Follow existing patterns (helper functions, target naming, generator expression style).
3. After making changes, launch the **qa-fixer** agent to format and fix any lint/spelling issues.
