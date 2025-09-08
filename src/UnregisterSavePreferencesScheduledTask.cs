using System.Management.Automation;
using System.Runtime.CompilerServices;
using System.Runtime.Versioning;

[assembly: InternalsVisibleTo("PSWinPrefsTests")]
namespace WinPrefs {
    [Alias("winprefs-uninstall-job")]
    [Cmdlet("Unregister", "SavePreferencesScheduledTask")]
    [SupportedOSPlatform("windows")]
    public class UnregisterSavePreferencesScheduledTask : PSCmdlet {
        [Alias("c")]
        [Parameter(HelpMessage = "Commit the changes with Git.")]
        public SwitchParameter Commit { get; set; } = false;

        [Alias("K")]
        [Parameter(HelpMessage = "Deploy key file path.")]
        public string? DeployKey;

        [Parameter(HelpMessage = "Output format.")]
        [ValidatePattern("^(reg|ps1?|cs|c#|c)$")]
        public string Format = "reg";

        [Alias("m")]
        [Parameter(HelpMessage = "Depth limit.")]
        public int MaxDepth = 20;

        [Alias("o")]
        [Parameter(HelpMessage = "Output directory.")]
        public string? OutputDirectory;

        [Alias("f")]
        [Parameter(HelpMessage = "Output filename.")]
        public string OutputFile = "exec-reg.bat";

        [Parameter(HelpMessage = "Full registry path.")]
        public string Path = "HKCU:\\";

        internal IScheduledTaskManager taskManager = new ScheduledTaskManager();
        private ScheduledTaskSuffix suffix = new ScheduledTaskSuffix();

        protected override void ProcessRecord() {
            var theSuffix = suffix.Get(Commit, DeployKey, OutputDirectory, OutputFile, Path, Format,
                MaxDepth);
            taskManager.unregister("tat.sh\\WinPrefs", $"WinPrefs-{theSuffix}");
        }

        internal void ProcessInternal() {
            BeginProcessing();
            ProcessRecord();
            EndProcessing();
        }
    }
}
