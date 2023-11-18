using System.Management.Automation;
using System.Runtime.Versioning;
using System.Security.Cryptography;
using System.Text;

using Microsoft.Win32.TaskScheduler;

namespace WinPrefs {
    [Alias("winprefs-install-job")]
    [Cmdlet("Register", "SavePreferencesScheduledTask")]
    [SupportedOSPlatform("windows")]
    public class RegisterSavePreferencesScheduledTask : PSCmdlet {
        [Alias("m")]
        [Parameter(HelpMessage = "Depth limit.")]
        public int MaxDepth = 20;

        [Alias("c")]
        [Parameter(HelpMessage = "Commit the changes with Git.")]
        public SwitchParameter Commit { get; set; } = false;

        [Alias("K")]
        [Parameter(HelpMessage = "Deploy key file path.")]
        public string? DeployKey;

        [Parameter(HelpMessage = "Output format.")]
        [ValidatePattern("^(reg|ps1?|cs|c#|c)$")]
        public string Format = "reg";

        [Alias("o")]
        [Parameter(HelpMessage = "Output directory.")]
        public string? OutputDirectory;

        [Alias("f")]
        [Parameter(HelpMessage = "Output filename.")]
        public string OutputFile = "exec-reg.bat";

        [Parameter(HelpMessage = "Full registry path.")]
        public string Path = @"HKCU:\";

        private string getSuffix() {
            using var sha1 = SHA1.Create();
            return Convert.ToHexString(sha1.ComputeHash(
                Encoding.UTF8.GetBytes(
               $"c={Commit},K={DeployKey},o={OutputDirectory},f={OutputFile},p={Path},f={Format}," +
               $"m={MaxDepth}")));
        }

        protected override void ProcessRecord() {
            using (TaskService ts = new TaskService()) {
                TaskFolder folder = ts.RootFolder.CreateFolder(@"tat.sh\WinPrefs\");
                TaskDefinition td = ts.NewTask();
                td.RegistrationInfo.Description = $"Runs WinPrefs every 12 hours (path {Path}).";
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
                td.Settings.ExecutionTimeLimit = TimeSpan.FromHours(2);
                td.Settings.StartWhenAvailable = true;
                td.Actions.Add(new ExecAction(
                    Environment.GetFolderPath(
                      Environment.SpecialFolder.CommonApplicationData) +
                      @"\WinPrefs\winprefsw.exe",
                    String.Join(" ", args),
                    Environment.GetFolderPath(Environment.SpecialFolder.UserProfile)));
                folder.RegisterTaskDefinition($"WinPrefs-{getSuffix()}", td);
            }
        }
    }
}
