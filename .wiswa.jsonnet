local utils = import 'utils.libjsonnet';

{
  uses_user_defaults: true,
  project_type: 'c',
  security_policy_supported_versions: { '0.4.x': ':white_check_mark:' },
  project_name: 'winprefs',
  version: '0.4.1',
  description: 'Tool to export registry paths to script and code formats (reg add, PowerShell, C, C#).',
  keywords: ['backup', 'batch', 'c', 'csharp', 'customisation', 'customization', 'desktop', 'powershell', 'registry', 'win32', 'windows'],
  clang_format_args: 'native/*.c native/*.h native/tests/*.c native/tests/*.h',
  want_main: false,
  want_codeql: false,
  want_tests: false,
  cz+: {
    commitizen+: {
      remove_path_prefixes+: ['native'],
      version_files+: [
        'man/winprefs.1',
        'src/WinPrefs.psd1',
      ],
    },
  },
  dependabot+: {
    updates+: [{
      cooldown: { 'default-days': 7 },
      directory: '/',
      'package-ecosystem': 'dotnet-sdk',
      schedule: { interval: 'weekly' },
    }],
  },
  shared_ignore+: ['/src/obj/'],
  gitignore+: ['*.ps1', '*.psd1', '/.vs/'],
  prettierignore+: ['*.cs'],
  package_json+: {
    prettier+: {
      overrides+: [{
        files: '*.csproj',
        options: {
          parser: 'xml',
          xmlWhitespaceSensitivity: 'ignore',
        },
      }],
    },
  },
  vscode+: {
    c_cpp+: {
      configurations: [
        {
          cStandard: 'gnu23',
          compilerPath: '/usr/bin/gcc',
          configurationProvider: 'ms-vscode.cmake-tools',
          cppStandard: 'gnu++23',
          includePath: ['${workspaceFolder}/native/**', '/usr/include/wine'],
          name: 'Linux',
        },
      ],
    },
    extensions+: {
      recommendations+: [
        'ms-vscode.powershell',
      ],
      unwantedRecommendations: ['ms-dotnettools.csdevkit'],
    },
    launch+: {
      configurations: [{
        cwd: '${workspaceRoot}',
        name: 'Run tests',
        program: '${workspaceRoot}/build/native/tests/test-output_reg_command',
        request: 'launch',
        setupCommands: [{ text: '-gdb-set follow-fork-mode child' }],
        type: 'cppdbg',
      }],
    },
    settings+: {
      '[csharp]': {
        'editor.defaultFormatter': 'ms-dotnettools.csharp',
      },
      '[powershell]': {
        'editor.defaultFormatter': 'ms-vscode.powershell',
      },
      'cmake.configureArgs': [
        '-DBUILD_DOCS=OFF',
        '-DBUILD_TESTS=ON',
        '-DCMAKE_BUILD_TYPE=Debug',
        '-DCMAKE_VERBOSE_MAKEFILE=ON',
        '-DENABLE_ASAN=ON',
        '-DENABLE_COVERAGE=ON',
        '-DENABLE_UBSAN=ON',
      ],
      'dotnet.server.useOmnisharp': true,
      'files.associations'+: {
        '*.h': 'c',
      },
    },
  },
  custom_project_badges: [
    {
      anchor: '[![PowerShell Gallery Version (including pre-releases)](https://img.shields.io/powershellgallery/v/WinPrefs)]',
      href: 'https://www.powershellgallery.com/packages/WinPrefs',
      priority: -1,
    },
    {
      anchor: '[![PowerShell Gallery Platform Support](https://img.shields.io/powershellgallery/p/WinPrefs?label=powershell+platforms+supported)]',
      href: 'https://www.powershellgallery.com/packages/WinPrefs',
      priority: -1,
    },
    {
      anchor: '[![PowerShell Gallery Downloads](https://img.shields.io/powershellgallery/dt/WinPrefs)]',
      href: 'https://www.powershellgallery.com/packages/WinPrefs',
      priority: -1,
    },
    {
      anchor: '[![Build](https://github.com/Tatsh/winprefs/actions/workflows/cmake.yml/badge.svg)]',
      href: 'https://github.com/Tatsh/winprefs/actions/workflows/cmake.yml',
    },
    {
      anchor: '[![CodeQL](https://github.com/Tatsh/winprefs/actions/workflows/codeql.yml/badge.svg)]',
      href: 'https://github.com/Tatsh/winprefs/actions/workflows/codeql.yml',
    },
    {
      anchor: '[![Tests](https://github.com/Tatsh/winprefs/actions/workflows/tests.yml/badge.svg)]',
      href: 'https://github.com/Tatsh/winprefs/actions/workflows/tests.yml',
    },
    {
      anchor: '[![Coverage Status](https://coveralls.io/repos/github/Tatsh/winprefs/badge.svg?branch=master)]',
      href: 'https://coveralls.io/github/Tatsh/winprefs?branch=master',
    },
  ],
  // C/C++ only
  vcpkg+: {
    dependencies: [{
      name: 'cmocka',
      platform: 'linux|mingw',
    }],
  },
}
