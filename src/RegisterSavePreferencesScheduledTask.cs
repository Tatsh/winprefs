using System.Management.Automation;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.Versioning;
using System.Text.RegularExpressions;
using Microsoft.Win32.TaskScheduler;

[assembly: InternalsVisibleTo("PSWinPrefsTests")]
namespace WinPrefs {
    [Alias("winprefs-install-job")]
    [Cmdlet("Register", "SavePreferencesScheduledTask")]
    [SupportedOSPlatform("windows")]
    public partial class RegisterSavePreferencesScheduledTask : PSCmdlet {
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

        internal IScheduledTaskManager taskManager = new ScheduledTaskManager();
        internal ICopyWinprefsw copier = new CopyWinprefsw();
        internal string? assemblyLocation = null;
        private readonly ScheduledTaskSuffix suffix = new();

        protected override void ProcessRecord() {
            var theSuffix = suffix.Get(Commit, DeployKey, OutputDirectory, OutputFile, Path, Format,
                MaxDepth);
            using TaskService ts = new();
            TaskDefinition td = ts.NewTask();
            td.RegistrationInfo.Description = $"Runs WinPrefs every 12 hours (path {Path}).";
            DailyTrigger trigger = new() {
                StartBoundary = DateTime.Today.AddDays(1)
            };
            trigger.Repetition.Interval = TimeSpan.FromHours(12);
            td.Triggers.Add(trigger);
            string[] args = [
                    Commit ? "-c" : "",
                    $"-m {MaxDepth}",
                    DeployKey != null ? $"-K \"{DeployKey}\"" : "",
                    OutputDirectory != null ? $"-o \"{OutputDirectory}\"" : "",
                    $"-f \"{OutputFile}\"",
                    $"-F {Format}",
                    Path
                ];
            td.Settings.ExecutionTimeLimit = TimeSpan.FromHours(2);
            string location = copier.Copy(
                assemblyLocation ?? Assembly.GetExecutingAssembly().Location,
                (ex) => ThrowTerminatingError(new ErrorRecord(
                        ex,
                        "AssemblyLocationError",
                        ErrorCategory.ResourceUnavailable,
                        this)));
            td.Settings.StartWhenAvailable = true;
            td.Actions.Add(new ExecAction(
                location,
                WhitespaceRegex().Replace(String.Join(" ", args), " "),
                Environment.GetFolderPath(Environment.SpecialFolder.UserProfile
            )));
            taskManager.register("tat.sh\\WinPrefs", $"WinPrefs-{theSuffix}", td);
        }

        internal void ProcessInternal() {
            BeginProcessing();
            ProcessRecord();
            EndProcessing();
        }

        [GeneratedRegex(@"\s+")]
        private static partial Regex WhitespaceRegex();
    }
}
