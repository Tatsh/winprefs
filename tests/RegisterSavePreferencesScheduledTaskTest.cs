using System.Diagnostics.CodeAnalysis;
using System.Management.Automation;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;
using Microsoft.Win32.TaskScheduler;
using Moq;
using Xunit;

namespace WinPrefs.Tests {
    [ExcludeFromCodeCoverageAttribute]
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
            var mockTaskManager = new Mock<IScheduledTaskManager>();
            mockTaskManager.Setup(tm => tm.register(
               It.IsAny<string>(), It.IsAny<string>(), It.IsAny<TaskDefinition>()));
            var mockCopyWinprefsw = new Mock<ICopyWinprefsw>();
            cmdlet.taskManager = mockTaskManager.Object;
            cmdlet.copier = mockCopyWinprefsw.Object;

            // Act
            cmdlet.ProcessInternal();

            // Assert
            mockTaskManager.Verify(tm =>
                tm.register("tat.sh\\WinPrefs",
                    It.Is<string>(s => s.StartsWith("WinPrefs-")), It.IsAny<TaskDefinition>()),
                    Times.Once);
        }

        private static void Thrower(Exception e) {
            throw e;
        }

        [Fact]
        public void Invoke_ShouldThrowError_WhenAssemblyLocationIsNull() {
            // Arrange
            var cmdlet = new RegisterSavePreferencesScheduledTask();
            var mockCopyWinprefsw = new Mock<ICopyWinprefsw>();
            cmdlet.assemblyLocation = "D:";

            // Act & Assert
            var ex = Xunit.Assert.Throws<Exception>(() => cmdlet.ProcessInternal());
            Xunit.Assert.Equal("Failed to get assembly location.", ex.Message);
        }
    }
}
