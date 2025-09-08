using System.Diagnostics.CodeAnalysis;
using System.Runtime.Versioning;
using Microsoft.Win32.TaskScheduler;
using Moq;
using Xunit;

namespace WinPrefs.Tests {
    [ExcludeFromCodeCoverageAttribute]
    [SupportedOSPlatform("windows")]
    public class UnregisterSavePreferencesScheduledTaskTest {
        [Fact]
        public void Invoke_ShouldNotThrow_WhenTatshFolderDoesNotExist() {
            // Arrange
            var cmdlet = new UnregisterSavePreferencesScheduledTask();
            var mockTaskManager = new Mock<IScheduledTaskManager>();
            mockTaskManager.Setup(tm => tm.unregister(
                It.IsAny<string>(), It.IsAny<string>()));
            cmdlet.taskManager = mockTaskManager.Object;

            // Act & Assert
            cmdlet.ProcessInternal();

            mockTaskManager.Verify(tm =>
                tm.unregister("tat.sh\\WinPrefs",
                    It.Is<string>(s => s.StartsWith("WinPrefs-"))),
                    Times.Once);
        }
    }
}
