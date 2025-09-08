using System.Diagnostics.CodeAnalysis;
using Microsoft.Win32.TaskScheduler;

namespace WinPrefs {
    [ExcludeFromCodeCoverageAttribute]
    public class ScheduledTaskManager : IScheduledTaskManager {
        public void register(string folderName, string taskName, TaskDefinition td) {
            using TaskService ts = new();
            var folder = ts.RootFolder.CreateFolder(folderName, null, false);
            folder.RegisterTaskDefinition(taskName, td);
        }

        public void unregister(string folderName, string taskName) {
            using TaskService ts = new();
            var parts = folderName.Split('\\');
            var rootColl = ts.RootFolder.SubFolders;
            TaskFolder? winprefsFolder = null;
            for (int i = 0; i < parts.Length; i++) {
                if ((i == parts.Length - 1) && rootColl.Exists(parts[i])) {
                    winprefsFolder = rootColl[parts[i]];
                    break;
                }
                if (!rootColl.Exists(parts[i])) {
                    return;
                }
                rootColl = rootColl[parts[i]].SubFolders;
            }
            Microsoft.Win32.TaskScheduler.Task task = ts.GetTask($"{folderName}\\{taskName}");
            if (task == null || winprefsFolder == null) {
                return;
            }
            winprefsFolder.DeleteTask(task.Name);
            // If everything is empty under tat.sh\WinPrefs delete the directories.
            if (winprefsFolder.Tasks.Count == 0) {
                while (true) {
                    var lastFolder = winprefsFolder;
                    winprefsFolder = winprefsFolder.Parent;
                    if (winprefsFolder == null || lastFolder.SubFolders.Count != 0 ||
                        lastFolder.Tasks.Count != 0) {
                        // Root or there are still children.
                        break;
                    }
                    winprefsFolder.DeleteFolder(lastFolder.Name);
                }
            }
        }
    }
}
