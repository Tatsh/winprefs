using System.Management.Automation;
using System.Runtime.CompilerServices;
using System.Runtime.Versioning;

[assembly: InternalsVisibleTo("PSWinPrefsTests")]
namespace WinPrefs {
    [Alias("prefs-export")]
    [Cmdlet("Save", "Preferences")]
    [SupportedOSPlatform("windows")]
    public class SavePreferences : PSCmdlet {
        [Alias("c")]
        [Parameter(HelpMessage = "Commit the changes with Git.")]
        public SwitchParameter Commit { get; set; } = false;

        [Alias("K")]
        [Parameter(HelpMessage = "Deploy key file path.")]
        public string? DeployKey;

        [Parameter(HelpMessage = "Output format.")]
        [ValidatePattern("^(reg|ps1?|powershell|cs|c#|c)$")]
        public string Format = "reg";

        [Alias("m")]
        [Parameter(HelpMessage = "Depth limit.")]
        public int MaxDepth = 20;

        [Alias("o")]
        [Parameter(HelpMessage = "Output directory.")]
        public string? OutputDirectory;

        [Parameter(HelpMessage = "Output filename.")]
        [Alias("f")]
        public string OutputFile = "exec-reg.bat";

        [Parameter(HelpMessage = "Full registry path.")]
        public string Path = "HKCU:\\";

        [Alias("S")]
        [Parameter(HelpMessage = "Do not read settings.")]
        public SwitchParameter NoSettings { get; set; } = false;

        internal LibPrefs? prefs = null;

        private bool IsDebugMode() {
            try {
                return MyInvocation.BoundParameters.TryGetValue("Debug", out object? value) ?
                    ((SwitchParameter)value).ToBool() :
                    ((ActionPreference)GetVariableValue("DebugPreference")
                        != ActionPreference.SilentlyContinue);
            } catch (NullReferenceException) {
                return false;
            }
        }

        protected override void BeginProcessing() {
            base.BeginProcessing();
            LibPrefs.SetDebugPrintEnabled(IsDebugMode());
        }

        protected override void EndProcessing() {
            base.EndProcessing();
            LibPrefs.SetDebugPrintEnabled(false);
        }

        protected override void ProcessRecord() {
            if (OutputDirectory == null) {
                string path = Environment.GetFolderPath(
                    Environment.SpecialFolder.ApplicationData);
                OutputDirectory = $"{path}\\prefs-export";
            }
            Directory.CreateDirectory(OutputDirectory);
            LibPrefs prefs = this.prefs ?? new(new UnsafeHandleUtil());
            if (!prefs.SavePreferences(LibPrefs.GetTopKey(Path),
                                          WriteObject,
                                          OutputFile == "-",
                                          Commit.ToBool(),
                                          !NoSettings.ToBool(),
                                          DeployKey,
                                          OutputDirectory,
                                          OutputFile,
                                          MaxDepth,
                                          Path,
                                          LibPrefs.ToEnum(Format)))
                ThrowTerminatingError(new ErrorRecord(
                    new Exception("Failed to save."), "WinPrefs_SavePreferencesError",
                    ErrorCategory.NotSpecified, null));
        }

        internal void ProcessInternal() {
            BeginProcessing();
            ProcessRecord();
            EndProcessing();
        }
    }
}
