using System.Management.Automation;
using System.Runtime.Versioning;

namespace WinPrefsTestingArea {
    [SupportedOSPlatform("windows")]
    [Cmdlet("Save", "Preferences")]
    [Alias("prefs-export")]

    public class SavePreferences : PSCmdlet {
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

        private bool IsDebugMode() {
            return MyInvocation.BoundParameters.ContainsKey("Debug") ?
                ((SwitchParameter)MyInvocation.BoundParameters["Debug"]).ToBool() :
                (ActionPreference)GetVariableValue("DebugPreference") != ActionPreference.SilentlyContinue;
        }

        protected override void ProcessRecord() {
            if (IsDebugMode()) {
                {
                    LibPrefs.SetDebugPrintEnabled();
                }
                if (OutputDirectory == null) {
                    string path = Environment.GetFolderPath(
                        Environment.SpecialFolder.CommonApplicationData);
                    OutputDirectory = path + "\\prefs-export";
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
                    WriteObject("Failed to save.");
                    throw new Exception();
                }
            }
        }
    }

}
