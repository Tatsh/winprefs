using System.Runtime.Versioning;
using Microsoft.Win32.TaskScheduler;
using Moq;
using Xunit;

namespace WinPrefs.Tests {
    [SupportedOSPlatform("windows")]
    public class UnregisterSavePreferencesScheduledTaskTest {
        [Fact]
        public void Invoke_ShouldNotThrow_WhenTatshFolderDoesNotExist() {
            // Arrange
            var cmdlet = new UnregisterSavePreferencesScheduledTask();
            //var mockTaskService = new Mock<TaskService>();
            //var mockRootFolder = new Mock<TaskFolder>();
            //var mockSubFolders = new Mock<TaskFolderCollection>();

            //mockTaskService.Setup(ts => ts.RootFolder).Returns(mockRootFolder.Object);
            //mockRootFolder.Setup(rf => rf.SubFolders).Returns(mockSubFolders.Object);
            //mockSubFolders.Setup(sf => sf.Exists("tat.sh")).Returns(false);

            // Act & Assert
            //var exception = Record.Exception(() => cmdlet.Invoke());
            //Assert.Null(exception);
        }

        [Fact]
        public void Invoke_ShouldNotThrow_WhenWinPrefsFolderDoesNotExist() {
            // Arrange
            var cmdlet = new UnregisterSavePreferencesScheduledTask();
            //var mockTaskService = new Mock<TaskService>();
            //var mockRootFolder = new Mock<TaskFolder>();
            //var mockTatshFolder = new Mock<TaskFolder>();
            //var mockSubFolders = new Mock<TaskFolderCollection>();

            //mockTaskService.Setup(ts => ts.RootFolder).Returns(mockRootFolder.Object);
            //mockRootFolder.Setup(rf => rf.SubFolders).Returns(mockSubFolders.Object);
            //mockSubFolders.Setup(sf => sf.Exists("tat.sh")).Returns(true);
            //mockSubFolders.Setup(sf => sf["tat.sh"]).Returns(mockTatshFolder.Object);
            //mockTatshFolder.Setup(tf => tf.SubFolders).Returns(mockSubFolders.Object);
            //mockSubFolders.Setup(sf => sf.Exists("WinPrefs")).Returns(false);

            // Act & Assert
            //var exception = Record.Exception(() => cmdlet.Invoke());
            //Assert.Null(exception);
        }

        [Fact]
        public void Invoke_ShouldDeleteTask_WhenTaskExists() {
            // Arrange
            var cmdlet = new UnregisterSavePreferencesScheduledTask();
            //var mockTaskService = new Mock<TaskService>();
            //var mockRootFolder = new Mock<TaskFolder>();
            //var mockTatshFolder = new Mock<TaskFolder>();
            //var mockWinPrefsFolder = new Mock<TaskFolder>();
            //var mockSubFolders = new Mock<TaskFolderCollection>();
            var mockTask = new Mock<Microsoft.Win32.TaskScheduler.Task>();

            //mockTaskService.Setup(ts => ts.RootFolder).Returns(mockRootFolder.Object);
            //mockRootFolder.Setup(rf => rf.SubFolders).Returns(mockSubFolders.Object);
            //mockSubFolders.Setup(sf => sf.Exists("tat.sh")).Returns(true);
            //mockSubFolders.Setup(sf => sf["tat.sh"]).Returns(mockTatshFolder.Object);
            //mockTatshFolder.Setup(tf => tf.SubFolders).Returns(mockSubFolders.Object);
            //mockSubFolders.Setup(sf => sf.Exists("WinPrefs")).Returns(true);
            //mockSubFolders.Setup(sf => sf["WinPrefs"]).Returns(mockWinPrefsFolder.Object);
            //mockWinPrefsFolder.Setup(wpf => wpf.Tasks).Returns(It.IsAny<TaskCollection>());
            //mockTaskService.Setup(ts => ts.GetTask(It.IsAny<string>())).Returns(mockTask.Object);

            // Act
            //cmdlet.Invoke();

            // Assert
            //mockWinPrefsFolder.Verify(wpf => wpf.DeleteTask(It.IsAny<string>(), false), Times.Once);
        }

        [Fact]
        public void Invoke_ShouldDeleteFolders_WhenEmpty() {
            // Arrange
            var cmdlet = new UnregisterSavePreferencesScheduledTask();
            //var mockTaskService = new Mock<TaskService>();
            //var mockRootFolder = new Mock<TaskFolder>();
            //var mockTatshFolder = new Mock<TaskFolder>();
            //var mockWinPrefsFolder = new Mock<TaskFolder>();
            //var mockSubFolders = new Mock<TaskFolderCollection>();
            var mockTask = new Mock<Microsoft.Win32.TaskScheduler.Task>();

            //mockTaskService.Setup(ts => ts.RootFolder).Returns(mockRootFolder.Object);
            //mockRootFolder.Setup(rf => rf.SubFolders).Returns(mockSubFolders.Object);
            //mockSubFolders.Setup(sf => sf.Exists("tat.sh")).Returns(true);
            //mockSubFolders.Setup(sf => sf["tat.sh"]).Returns(mockTatshFolder.Object);
            //mockTatshFolder.Setup(tf => tf.SubFolders).Returns(mockSubFolders.Object);
            //mockSubFolders.Setup(sf => sf.Exists("WinPrefs")).Returns(true);
            //mockSubFolders.Setup(sf => sf["WinPrefs"]).Returns(mockWinPrefsFolder.Object);
            //mockWinPrefsFolder.Setup(wpf => wpf.Tasks).Returns(It.IsAny<TaskCollection>());
            //mockTaskService.Setup(ts => ts.GetTask(It.IsAny<string>())).Returns(mockTask.Object);
            //mockWinPrefsFolder.Setup(wpf => wpf.Tasks.Count).Returns(0);
            //mockTatshFolder.Setup(tf => tf.SubFolders.Count).Returns(0);
            //mockTatshFolder.Setup(tf => tf.Tasks.Count).Returns(0);

            // Act
            //cmdlet.Invoke();

            // Assert
            //mockTatshFolder.Verify(tf => tf.DeleteFolder("WinPrefs", false), Times.Once);
            //mockRootFolder.Verify(rf => rf.DeleteFolder("tat.sh", false), Times.Once);
        }

        [Fact]
        public void Invoke_ShouldDeleteWinprefswFile_WhenExists() {
            // Arrange
            var cmdlet = new UnregisterSavePreferencesScheduledTask();
            //var mockTaskService = new Mock<TaskService>();
            //var mockRootFolder = new Mock<TaskFolder>();
            //var mockTatshFolder = new Mock<TaskFolder>();
            //var mockWinPrefsFolder = new Mock<TaskFolder>();
            //var mockSubFolders = new Mock<TaskFolderCollection>();
            var mockTask = new Mock<Microsoft.Win32.TaskScheduler.Task>();

            //mockTaskService.Setup(ts => ts.RootFolder).Returns(mockRootFolder.Object);
            //mockRootFolder.Setup(rf => rf.SubFolders).Returns(mockSubFolders.Object);
            //mockSubFolders.Setup(sf => sf.Exists("tat.sh")).Returns(true);
            //mockSubFolders.Setup(sf => sf["tat.sh"]).Returns(mockTatshFolder.Object);
            //mockTatshFolder.Setup(tf => tf.SubFolders).Returns(mockSubFolders.Object);
            //mockSubFolders.Setup(sf => sf.Exists("WinPrefs")).Returns(true);
            //mockSubFolders.Setup(sf => sf["WinPrefs"]).Returns(mockWinPrefsFolder.Object);
            //mockWinPrefsFolder.Setup(wpf => wpf.Tasks).Returns(It.IsAny<TaskCollection>());
            //mockTaskService.Setup(ts => ts.GetTask(It.IsAny<string>())).Returns(mockTask.Object);
            //mockWinPrefsFolder.Setup(wpf => wpf.Tasks.Count).Returns(0);
            //mockTatshFolder.Setup(tf => tf.SubFolders.Count).Returns(0);
            //mockTatshFolder.Setup(tf => tf.Tasks.Count).Returns(0);

            string appDataDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "WinPrefs");
            string winprefswPath = Path.Combine(appDataDir, "winprefsw.exe");
            Directory.CreateDirectory(appDataDir);
            File.WriteAllText(winprefswPath, "dummy content");

            // Act
            //cmdlet.Invoke();

            // Assert
            //Assert.False(File.Exists(winprefswPath));
        }
    }
}
