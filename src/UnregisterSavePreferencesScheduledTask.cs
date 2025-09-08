using Microsoft.Win32.TaskScheduler;
using System.Management.Automation;
using System.Runtime.CompilerServices;
using System.Runtime.Versioning;
using System.Security.Cryptography;
using System.Text;
using IOPath = System.IO.Path;

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

        private string GetSuffix() {
            using var sha1 = SHA1.Create();
            return Convert.ToHexString(sha1.ComputeHash(
                Encoding.UTF8.GetBytes(
               $"c={Commit},K={DeployKey},o={OutputDirectory},f={OutputFile},p={Path}," +
               $"f={Format},m={MaxDepth}")));
        }

        protected override void ProcessRecord() {
            using TaskService ts = new();
            TaskFolderCollection rootColl = ts.RootFolder.SubFolders;
            if (!rootColl.Exists("tat.sh")) {
                return;
            }
            TaskFolder tatshFolder = ts.RootFolder.SubFolders["tat.sh"];
            if (!tatshFolder.SubFolders.Exists("WinPrefs")) {
                return;
            }
            TaskFolder winprefsFolder = tatshFolder.SubFolders["WinPrefs"];
            string taskName = $"tat.sh\\WinPrefs\\WinPrefs-{GetSuffix()}";
            Microsoft.Win32.TaskScheduler.Task task = ts.GetTask(taskName);
            if (task == null) {
                return;
            }
            winprefsFolder.DeleteTask(task.Name);
            // If everything is empty under tat.sh\WinPrefs delete the directories.
            if (winprefsFolder.Tasks.Count == 0) {
                tatshFolder.DeleteFolder("WinPrefs");
                string appDataDir = IOPath.Combine(Environment.GetFolderPath(
                  Environment.SpecialFolder.LocalApplicationData), "WinPrefs");
                string winprefswPath = IOPath.Combine(appDataDir, "winprefsw.exe");
                if (File.Exists(winprefswPath)) {
                    File.Delete(winprefswPath);
                }
                if (Directory.GetFiles(appDataDir).Length == 0) {
                    Directory.Delete(appDataDir);
                }
            }
            // Again for tat.sh
            if (tatshFolder.SubFolders.Count == 0 && tatshFolder.Tasks.Count == 0) {
                ts.RootFolder.DeleteFolder("tat.sh");
            }
        }
        internal void ProcessInternal() {
            BeginProcessing();
            ProcessRecord();
            EndProcessing();
        }
    }
}
