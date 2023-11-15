using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Management.Automation;
using System.Runtime.Versioning;

namespace WinPrefsTestingArea
{
    [SupportedOSPlatform("windows")]
    [Cmdlet("Unregister", "SavePreferencesScheduledTask")]
    [Alias("winprefs-uninstall-job")]
    public class UnregisterSavePreferencesScheduledTask : PSCmdlet
    {
        [Parameter(HelpMessage = "Depth limit.")]
        [Alias("m")]
        public int MaxDepth = 20;

        [Parameter(HelpMessage = "Commit the changes with Git.")]
        [Alias("c")]
        public SwitchParameter Commit { get; set; } = false;

        [Parameter(HelpMessage = "Deploy key file path.")]
        [Alias("K")]
        public string? DeployKey;

        [Parameter(HelpMessage = "Output directory.")]
        [Alias("o")]
        public string? OutputDirectory;

        [Parameter(HelpMessage = "Output filename.")]
        [Alias("-f")]
        public string OutputFile = "exec-reg.bat";

        [Parameter(HelpMessage = "Full registry path.")]
        public string Path = "HKCU:\\";

        [Parameter(HelpMessage = "Output format.")]
        [Alias("-F")]
        [ValidatePattern("^(reg|ps1?|cs|c#|c)$")]
        public string Format = "reg";

        protected override void ProcessRecord()
        { }
    }
}
