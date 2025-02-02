using System;
using System.IO;
using System.Management.Automation;
using System.Reflection;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;
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

            var mockTaskService = new Mock<TaskService>();
            var mockTaskFolder = new Mock<TaskFolder>();
            var mockTaskDefinition = new Mock<TaskDefinition>();
            var mockDailyTrigger = new Mock<DailyTrigger>();
            var mockExecAction = new Mock<ExecAction>();

            mockTaskService.Setup(ts => ts.RootFolder).Returns(mockTaskFolder.Object);
            mockTaskService.Setup(ts => ts.NewTask()).Returns(mockTaskDefinition.Object);
            mockTaskDefinition.Setup(td => td.Triggers).Returns(new TriggerCollection(mockTaskDefinition.Object));
            mockTaskDefinition.Setup(td => td.Actions).Returns(new ActionCollection(mockTaskDefinition.Object));
            mockTaskDefinition.Setup(td => td.RegistrationInfo).Returns(new RegistrationInfo(mockTaskDefinition.Object));
            mockTaskDefinition.Setup(td => td.Settings).Returns(new TaskSettings(mockTaskDefinition.Object));

            // Act
            cmdlet.Invoke();

            // Assert
            mockTaskFolder.Verify(tf => tf.CreateFolder(@"tat.sh\WinPrefs", null, false), Times.Once);
            mockTaskDefinition.VerifySet(td => td.RegistrationInfo.Description = "Runs WinPrefs every 12 hours (path HKCU:\\SomePath).");
            mockTaskDefinition.Verify(td => td.Triggers.Add(It.IsAny<DailyTrigger>()), Times.Once);
            mockTaskDefinition.Verify(td => td.Actions.Add(It.IsAny<ExecAction>()), Times.Once);
            mockTaskFolder.Verify(tf => tf.RegisterTaskDefinition(It.IsAny<string>(), mockTaskDefinition.Object), Times.Once);
        }

        [Fact]
        public void Invoke_ShouldThrowError_WhenAssemblyLocationIsNull() {
            // Arrange
            var cmdlet = new RegisterSavePreferencesScheduledTask();
            var mockTaskService = new Mock<TaskService>();
            var mockTaskFolder = new Mock<TaskFolder>();
            var mockTaskDefinition = new Mock<TaskDefinition>();

            mockTaskService.Setup(ts => ts.RootFolder).Returns(mockTaskFolder.Object);
            mockTaskService.Setup(ts => ts.NewTask()).Returns(mockTaskDefinition.Object);
            mockTaskDefinition.Setup(td => td.Triggers).Returns(new TriggerCollection(mockTaskDefinition.Object));
            mockTaskDefinition.Setup(td => td.Actions).Returns(new ActionCollection(mockTaskDefinition.Object));
            mockTaskDefinition.Setup(td => td.RegistrationInfo).Returns(new RegistrationInfo(mockTaskDefinition.Object));
            mockTaskDefinition.Setup(td => td.Settings).Returns(new TaskSettings(mockTaskDefinition.Object));

            // Act & Assert
            var exception = Assert.Throws<CmdletInvocationException>(() => cmdlet.Invoke());
            Assert.Equal("Failed to get assembly location.", exception.Message);
        }

        [Fact]
        public void Invoke_ShouldDeleteExistingWinprefswFile() {
            // Arrange
            var cmdlet = new RegisterSavePreferencesScheduledTask();
            var mockTaskService = new Mock<TaskService>();
            var mockTaskFolder = new Mock<TaskFolder>();
            var mockTaskDefinition = new Mock<TaskDefinition>();

            mockTaskService.Setup(ts => ts.RootFolder).Returns(mockTaskFolder.Object);
            mockTaskService.Setup(ts => ts.NewTask()).Returns(mockTaskDefinition.Object);
            mockTaskDefinition.Setup(td => td.Triggers).Returns(new TriggerCollection(mockTaskDefinition.Object));
            mockTaskDefinition.Setup(td => td.Actions).Returns(new ActionCollection(mockTaskDefinition.Object));
            mockTaskDefinition.Setup(td => td.RegistrationInfo).Returns(new RegistrationInfo(mockTaskDefinition.Object));
            mockTaskDefinition.Setup(td => td.Settings).Returns(new TaskSettings(mockTaskDefinition.Object));

            string appDataDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "WinPrefs");
            string winprefswPath = Path.Combine(appDataDir, "winprefsw.exe");
            Directory.CreateDirectory(appDataDir);
            File.WriteAllText(winprefswPath, "dummy content");

            // Act
            cmdlet.Invoke();

            // Assert
            Assert.False(File.Exists(winprefswPath));
        }
    }
}
