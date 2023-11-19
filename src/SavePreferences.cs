using System.Management.Automation;
using System.Runtime.Versioning;

namespace WinPrefs {
    [Alias("prefs-export")]
    [Cmdlet("Save", "Preferences")]
    [SupportedOSPlatform("windows")]
    public class SavePreferences : PSCmdlet {
        [Parameter(HelpMessage = "Commit the changes with Git.")]
        [Alias("c")]
        public SwitchParameter Commit { get; set; } = false;

        [Parameter(HelpMessage = "Deploy key file path.")]
        [Alias("K")]
        public string? DeployKey;

        [Parameter(HelpMessage = "Output format.")]
        [ValidatePattern("^(reg|ps1?|cs|c#|c)$")]
        public string Format = "reg";

        [Parameter(HelpMessage = "Depth limit.")]
        [Alias("m")]
        public int MaxDepth = 20;

        [Parameter(HelpMessage = "Output directory.")]
        [Alias("o")]
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

        protected override void ProcessRecord() {
            if (IsDebugMode()) {
                {
                    LibPrefs.SetDebugPrintEnabled();
                }
                if (OutputDirectory == null) {
                    string path = Environment.GetFolderPath(
                        Environment.SpecialFolder.CommonApplicationData);
                    OutputDirectory = $"{path}\\prefs-export";
                }
                Directory.CreateDirectory(OutputDirectory);
                if (!LibPrefs.SavePreferences(LibPrefs.GetTopKey(Path),
                                              Commit.ToBool(),
                                              DeployKey,
                                              OutputDirectory,
                                              OutputFile,
                                              MaxDepth,
                                              Path,
                                              LibPrefs.ToEnum(Format))) {
                    ThrowTerminatingError(new ErrorRecord(new Exception("Failed to save."), "WinPrefs_SavePreferencesError", ErrorCategory.NotSpecified, null));
                }
            }
        }
    }

}
