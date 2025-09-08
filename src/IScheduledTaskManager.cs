using Microsoft.Win32.TaskScheduler;

namespace WinPrefs {
    public interface IScheduledTaskManager {
        public void register(string folderName, string taskName, TaskDefinition td);
        public void unregister(string folderName, string taskName);
    }
}
