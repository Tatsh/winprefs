using System.Management.Automation;
using System.Runtime.Versioning;

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

        private bool IsDebugMode() {
            return MyInvocation.BoundParameters.ContainsKey("Debug") ?
                ((SwitchParameter)MyInvocation.BoundParameters["Debug"]).ToBool() :
                ((ActionPreference)GetVariableValue("DebugPreference")
                    != ActionPreference.SilentlyContinue);
        }

        protected override void BeginProcessing() {
            base.BeginProcessing();
            if (IsDebugMode()) {
                LibPrefs.SetDebugPrintEnabled();
            }
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
            LibPrefs prefs = new();
            if (!prefs.SavePreferences(LibPrefs.GetTopKey(Path),
                                          WriteObject,
                                          OutputFile == "-",
                                          Commit.ToBool(),
                                          DeployKey,
                                          OutputDirectory,
                                          OutputFile,
                                          MaxDepth,
                                          Path,
                                          LibPrefs.ToEnum(Format))) {
                ThrowTerminatingError(new ErrorRecord(
                    new Exception("Failed to save."), "WinPrefs_SavePreferencesError",
                    ErrorCategory.NotSpecified, null));
            }
        }
    }

}
