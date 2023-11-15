using System.Management.Automation;
using System.Runtime.Versioning;

using Microsoft.Win32;

namespace WinPrefsTestingArea {
    [SupportedOSPlatform("windows")]
    [Cmdlet(VerbsCommunications.Write, "RegCommands")]
    [Alias("reg2path")]
    public class WriteRegCommands : PSCmdlet {
        [Parameter(Mandatory = true, HelpMessage = "Registry path.")]
        [ValidatePattern("^HK(LM|CU|CR|U|CC):")]
        public string Path = "";

        [Parameter(HelpMessage = "Output format.")]
        [Alias("-F")]
        [ValidatePattern("^(reg|ps1?|cs|c#|c)$")]
        public string Format = "reg";

        [Parameter(HelpMessage = "Depth limit.")]
        [Alias("-m")]
        public int MaxDepth = 20;

        private bool IsDebugMode() {
            return MyInvocation.BoundParameters.ContainsKey("Debug") ?
                ((SwitchParameter)MyInvocation.BoundParameters["Debug"]).ToBool() :
                (ActionPreference)GetVariableValue("DebugPreference") != ActionPreference.SilentlyContinue;
        }

        protected override void ProcessRecord() {
            if (IsDebugMode()) {
                LibPrefs.SetDebugPrintEnabled();
            }
            RegistryKey? hk = LibPrefs.GetTopKey(Path);
            RegistryKey topKey = hk;
            string subkey = string.Join("\\", Path.Split("\\").Skip(1));
            WriteObject(subkey);
            if (subkey.Length > 0) {
                hk = hk.OpenSubKey(subkey);
                if (hk == null) {
                    if (!LibPrefs.ExportSingleValue(topKey, Path, LibPrefs.ToEnum(Format))) {
                        WriteObject("Failed to export " + Path + " as single value.");
                    }
                    throw new Exception();
                }
            }
            if (!LibPrefs.SavePreferences(outputFile: "-",
                                          maxDepth: MaxDepth,
                                          hk: hk,
                                          specifiedPath: Path,
                                          format: LibPrefs.ToEnum(Format))) {
                WriteObject("Failed to export " + Path + ".");
                throw new Exception();
            }
        }
    }
}
