using System.Management.Automation;
using System.Runtime.Versioning;
using System.Text.RegularExpressions;
using System.Security.Cryptography;
using System.Text;

using Microsoft.Win32.TaskScheduler;

namespace WinPrefs {
    [Alias("winprefs-install-job")]
    [Cmdlet("Register", "SavePreferencesScheduledTask")]
    [SupportedOSPlatform("windows")]
    public class RegisterSavePreferencesScheduledTask : PSCmdlet {
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

        [Parameter(HelpMessage = "Full registry path.")]
        public string Path = @"HKCU:\";

        [Parameter(HelpMessage = "Output format.")]
        [ValidatePattern("^(reg|ps1?|cs|c#|c)$")]
        public string Format = "reg";

        private string getSuffix() {
            using var sha1 = SHA1.Create();
            return Convert.ToHexString(sha1.ComputeHash(
                Encoding.UTF8.GetBytes(
               $"c={Commit},K={DeployKey},o={OutputDirectory},f={OutputFile},p={Path},f={Format},m={MaxDepth}")));
        }

        protected override void ProcessRecord() {
            using (TaskService ts = new TaskService()) {
                TaskFolder folder = ts.RootFolder.CreateFolder(@"tat.sh\WinPrefs\");
                TaskDefinition td = ts.NewTask();
                td.RegistrationInfo.Description = $"Run SavePreferences every 12 hours (path {Path}).";
                DailyTrigger trigger = new DailyTrigger();
                trigger.StartBoundary = DateTime.Today.AddDays(1);
                trigger.Repetition.Interval = TimeSpan.FromHours(12);
                td.Triggers.Add(trigger);
                string[] args = {
                    Commit ? "-c" : "",
                    $"-m {MaxDepth}",
                    DeployKey != null ? $"-K \"{DeployKey}\"" : "",
                    OutputDirectory != null ? $"-o \"{OutputDirectory}\"" : "",
                    $"-f \"{OutputFile}\"",
                    $"-F {Format}",
                    Path
                };
                string winprefswPath = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\WinPrefs\\winprefsw.exe";
                // td.Settings.MultipleInstances = "IgnoreNew";
                td.Settings.ExecutionTimeLimit = TimeSpan.FromHours(2);
                td.Settings.StartWhenAvailable = true;
                td.Actions.Add(new ExecAction(winprefswPath, String.Join(" ", args), Environment.GetFolderPath(Environment.SpecialFolder.UserProfile)));
                folder.RegisterTaskDefinition($"WinPrefs-{getSuffix()}", td);
            }
        }
    }
}
