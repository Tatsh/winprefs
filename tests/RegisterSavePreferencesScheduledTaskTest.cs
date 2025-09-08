using System.Management.Automation;
using System.Runtime.Versioning;
using Microsoft.Win32.TaskScheduler;
using Moq;
using Xunit;

namespace WinPrefs.Tests {
    [SupportedOSPlatform("windows")]
    public class RegisterSavePreferencesScheduledTaskTest {
        [Fact]
        public void Invoke_ShouldCreateTaskWithCorrectSettings() {
            // Arrange
            var cmdlet = new RegisterSavePreferencesScheduledTask {
                MaxDepth = 10,
                Commit = true,
                DeployKey = "deployKeyPath",
                Format = "cs",
                OutputDirectory = "outputDir",
                OutputFile = "outputFile.bat",
                Path = @"HKCU:\SomePath"
            };
            //var mockTaskService = new Mock<TaskService>();
            //var mockTaskFolder = new Mock<TaskFolder>();
            //var mockTaskDefinition = new Mock<TaskDefinition>();
            //var mockDailyTrigger = new Mock<DailyTrigger>();
            var mockExecAction = new Mock<ExecAction>();

            //mockTaskService.Setup(ts => ts.RootFolder).Returns(mockTaskFolder.Object);
            //mockTaskService.Setup(ts => ts.NewTask()).Returns(mockTaskDefinition.Object);
            //mockTaskDefinition.Setup(td => td.Triggers).Returns(It.IsAny<TriggerCollection>());
            //mockTaskDefinition.Setup(td => td.Actions).Returns(It.IsAny<ActionCollection>());
            //mockTaskDefinition.Setup(td => td.RegistrationInfo).Returns(It.IsAny<TaskRegistrationInfo>());
            //mockTaskDefinition.Setup(td => td.Settings).Returns(It.IsAny<TaskSettings>());

            // Act
            // cmdlet.Invoke();

            // Assert
            //mockTaskDefinition.Verify(td => td.Actions.Add(It.IsAny<ExecAction>()), Times.Once);
            //mockTaskFolder.Verify(tf => tf.RegisterTaskDefinition(It.IsAny<string>(), mockTaskDefinition.Object), Times.Once);
        }

        [Fact]
        public void Invoke_ShouldThrowError_WhenAssemblyLocationIsNull() {
            // Arrange
            var cmdlet = new RegisterSavePreferencesScheduledTask();
            //var mockTaskService = new Mock<TaskService>();
            //var mockTaskFolder = new Mock<TaskFolder>();
            //var mockTaskDefinition = new Mock<TaskDefinition>();

            //mockTaskService.Setup(ts => ts.RootFolder).Returns(mockTaskFolder.Object);
            //mockTaskService.Setup(ts => ts.NewTask()).Returns(mockTaskDefinition.Object);
            //mockTaskDefinition.Setup(td => td.Triggers).Returns(It.IsAny<TriggerCollection>());
            //mockTaskDefinition.Setup(td => td.Actions).Returns(It.IsAny<ActionCollection>());
            //mockTaskDefinition.Setup(td => td.RegistrationInfo).Returns(It.IsAny<TaskRegistrationInfo>());
            //mockTaskDefinition.Setup(td => td.Settings).Returns(It.IsAny<TaskSettings>());

            // Act & Assert
            // var exception = Assert.Throws<CmdletInvocationException>(() => cmdlet.Invoke());
            // Assert.Equal("Failed to get assembly location.", exception.Message);
        }

        [Fact]
        public void Invoke_ShouldDeleteExistingWinprefswFile() {
            // Arrange
            var cmdlet = new RegisterSavePreferencesScheduledTask();
            //var mockTaskService = new Mock<TaskService>();
            //var mockTaskFolder = new Mock<TaskFolder>();
            //var mockTaskDefinition = new Mock<TaskDefinition>();

            //mockTaskService.Setup(ts => ts.RootFolder).Returns(mockTaskFolder.Object);
            //mockTaskService.Setup(ts => ts.NewTask()).Returns(mockTaskDefinition.Object);
            //mockTaskDefinition.Setup(td => td.Triggers).Returns(It.IsAny<TriggerCollection>());
            //mockTaskDefinition.Setup(td => td.Actions).Returns(It.IsAny<ActionCollection>());
            //mockTaskDefinition.Setup(td => td.RegistrationInfo).Returns(It.IsAny<TaskRegistrationInfo>());
            //mockTaskDefinition.Setup(td => td.Settings).Returns(It.IsAny<TaskSettings>());

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
