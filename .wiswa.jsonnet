local utils = import 'utils.libjsonnet';

{
  project_type: 'c',
  security_policy_supported_versions: { '0.4.x': ':white_check_mark:' },
  project_name: 'winprefs',
  version: '0.4.0',
  description: 'Tool to export registry paths to script and code formats (reg add, PowerShell, C, C#).',
  keywords: ['backup', 'batch', 'c', 'csharp', 'customisation', 'customization', 'desktop', 'powershell', 'registry', 'win32', 'windows'],
  want_main: false,
  want_codeql: false,
  want_tests: false,
  copilot+: {
    intro: 'winprefs is a tool to export registry data to various programming languages.',
  },
  cz+: {
    commitizen+: {
      remove_path_prefixes+: ['native'],
      version_files+: [
        'man/winprefs.1',
        'src/WinPrefs.psd1',
      ],
    },
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
    scripts+: {
      'check-formatting': "clang-format -n native/*.c native/*.h native/tests/*.c native/tests/*.h && prettier -c . && markdownlint-cli2 '**/*.md' '#node_modules'",
      format: 'cmake-format -i CMakeLists.txt native/CMakeLists.txt; clang-format -i native/*.c native/*.h native/tests/*.c native/tests/*.h && yarn prettier -w .',

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
  // C/C++ only
  vcpkg+: {
    dependencies: [{
      name: 'cmocka',
      platform: 'linux|mingw',
    }],
  },
}
