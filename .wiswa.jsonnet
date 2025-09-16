local utils = import 'utils.libjsonnet';

(import 'defaults.libjsonnet') + {
  local top = self,
  // General settings
  project_type: 'c',

  // Shared
  github_username: 'Tatsh',
  security_policy_supported_versions: { '0.3.x': ':white_check_mark:' },
  authors: [
    {
      'family-names': 'Udvare',
      'given-names': 'Andrew',
      email: 'audvare@gmail.com',
      name: '%s %s' % [self['given-names'], self['family-names']],
    },
  ],
  project_name: 'winprefs',
  version: '0.4.0',
  description: 'Tool to export registry paths to script and code formats (reg add, PowerShell, C, C#).',
  keywords: ['backup', 'batch', 'c', 'csharp', 'customisation', 'customization', 'desktop', 'powershell', 'registry', 'win32', 'windows'],
  want_main: false,
  copilot: {
    intro: 'winprefs is a tool to export registry data to various programming languages.',
  },
  social+: {
    mastodon+: { id: '109370961877277568' },
  },

  // GitHub
  github+: {
    funding+: {
      ko_fi: 'tatsh2',
      liberapay: 'tatsh2',
      patreon: 'tatsh2',
    },
  },

  // C++ only
  vcpkg+: {
    dependencies: [{
      name: 'cmocka',
      platform: 'linux|mingw',
    }],
  },
}
