using System.Management.Automation;
using System.Runtime.Versioning;
using System.Security.Cryptography;
using System.Linq;
using System.Text;

using Microsoft.Win32.TaskScheduler;

namespace WinPrefs {
    [SupportedOSPlatform("windows")]
    [Cmdlet("Unregister", "SavePreferencesScheduledTask")]
    [Alias("winprefs-uninstall-job")]
    public class UnregisterSavePreferencesScheduledTask : PSCmdlet {
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
        [Alias("f")]
        public string OutputFile = "exec-reg.bat";

        [Parameter(HelpMessage = "Output format.")]
        [ValidatePattern("^(reg|ps1?|cs|c#|c)$")]
        public string Format = "reg";

        [Parameter(HelpMessage = "Full registry path.")]
        public string Path = "HKCU:\\";

        private string getSuffix() {
            using var sha1 = SHA1.Create();
            return Convert.ToHexString(sha1.ComputeHash(
                Encoding.UTF8.GetBytes(
               $"c={Commit},K={DeployKey},o={OutputDirectory},f={OutputFile},p={Path},f={Format},m={MaxDepth}")));
        }

        protected override void ProcessRecord() {
            string suffix = getSuffix();
            using (TaskService ts = new TaskService()) {
                Microsoft.Win32.TaskScheduler.Task task = ts.GetTask($"tat.sh\\WinPrefs\\WinPrefs-{getSuffix()}");
                if (task == null) {
                    return;
                }
                TaskFolderCollection rootColl = ts.RootFolder.SubFolders;
                if (!rootColl.Exists("tat.sh")) {
                    return;
                }
                TaskFolder tatshFolder = ts.RootFolder.SubFolders["tat.sh"];
                TaskFolder winprefsFolder = tatshFolder.SubFolders["WinPrefs"];
                // If everything is empty under tat.sh\WinPrefs delete the directories.
                if (tatshFolder.SubFolders.Exists("WinPrefs") && winprefsFolder.Tasks.Count == 0) {
                    tatshFolder.DeleteFolder("WinPrefs");
                }
                // Again for tat.sh
                if (tatshFolder.SubFolders.Count == 0 && tatshFolder.Tasks.Count == 0) {
                    ts.RootFolder.DeleteFolder("tat.sh");
                }
            }
        }
    }
}
