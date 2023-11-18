using System.Management.Automation;
using System.Runtime.Versioning;

using Microsoft.Win32;

namespace WinPrefs {
    [Alias("path2reg")]
    [Cmdlet(VerbsCommunications.Write, "RegCommands")]
    [SupportedOSPlatform("windows")]
    public class WriteRegCommands : PSCmdlet {
        [Alias("F")]
        [Parameter(HelpMessage = "Output format.")]
        [ValidatePattern("^(reg|ps1?|cs|c#|c)$")]
        public string Format = "reg";

        [Alias("m")]
        [Parameter(HelpMessage = "Depth limit.")]
        public int MaxDepth = 20;

        [Parameter(Mandatory = true, HelpMessage = "Registry path.")]
        [ValidatePattern("^HK(LM|CU|CR|U|CC):")]
        public string Path = "";

        private bool IsDebugMode() {
            return MyInvocation.BoundParameters.ContainsKey("Debug") ?
                ((SwitchParameter)MyInvocation.BoundParameters["Debug"]).ToBool() :
                ((ActionPreference)GetVariableValue("DebugPreference")
                    != ActionPreference.SilentlyContinue);
        }

        protected override void ProcessRecord() {
            if (IsDebugMode()) {
                LibPrefs.SetDebugPrintEnabled();
            }
            RegistryKey? hk = LibPrefs.GetTopKey(Path);
            RegistryKey topKey = hk;
            string subkey = string.Join("\\", Path.Split("\\").Skip(1));
            if (subkey.Length > 0) {
                hk = hk.OpenSubKey(subkey);
                if (hk == null) {
                    if (!LibPrefs.ExportSingleValue(topKey, Path, LibPrefs.ToEnum(Format))) {
                        throw new Exception();
                    }
                    return;
                }
            }
            if (!LibPrefs.SavePreferences(outputFile: "-",
                                          maxDepth: MaxDepth,
                                          hk: hk,
                                          specifiedPath: Path,
                                          format: LibPrefs.ToEnum(Format))) {
                WriteObject($"Failed to export {Path}.");
                throw new Exception();
            }
        }
    }
}
