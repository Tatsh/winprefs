using System;
using System.IO;
using System.Management.Automation;
using Microsoft.Win32;
using Moq;
using Xunit;

namespace WinPrefs.Tests {
    [SupportedOSPlatform("windows")]
    public class SavePreferencesTest {
        [Fact]
        public void Invoke_ShouldCreateOutputDirectory_WhenNotSpecified() {
            // Arrange
            var cmdlet = new SavePreferences();
            var mockLibPrefs = new Mock<LibPrefs>();
            string expectedOutputDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "prefs-export");

            // Act
            cmdlet.Invoke();

            // Assert
            Assert.Equal(expectedOutputDir, cmdlet.OutputDirectory);
            Assert.True(Directory.Exists(expectedOutputDir));
        }

        [Fact]
        public void Invoke_ShouldThrowError_WhenSavePreferencesFails() {
            // Arrange
            var cmdlet = new SavePreferences();
            var mockLibPrefs = new Mock<LibPrefs>();
            mockLibPrefs.Setup(lp => lp.SavePreferences(It.IsAny<RegistryKey>(),
                                                        It.IsAny<Action<object>>(),
                                                        It.IsAny<bool>(),
                                                        It.IsAny<bool>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<int>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<LibPrefs.OutputFormat>()))
                        .Returns(false);

            // Act & Assert
            var exception = Assert.Throws<CmdletInvocationException>(() => cmdlet.Invoke());
            Assert.Equal("Failed to save.", exception.Message);
        }

        [Fact]
        public void Invoke_ShouldCallSavePreferences_WithCorrectParameters() {
            // Arrange
            var cmdlet = new SavePreferences {
                Commit = true,
                DeployKey = "deployKeyPath",
                Format = "cs",
                OutputDirectory = "outputDir",
                OutputFile = "outputFile.bat",
                Path = @"HKCU:\SomePath"
            };
            var mockLibPrefs = new Mock<LibPrefs>();

            // Act
            cmdlet.Invoke();

            // Assert
            mockLibPrefs.Verify(lp => lp.SavePreferences(It.IsAny<RegistryKey>(),
                                                        It.IsAny<Action<object>>(),
                                                        false,
                                                        true,
                                                        "deployKeyPath",
                                                        "outputDir",
                                                        "outputFile.bat",
                                                        20,
                                                        @"HKCU:\SomePath",
                                                        LibPrefs.OutputFormat.CSharp), Times.Once);
        }
    }
}
