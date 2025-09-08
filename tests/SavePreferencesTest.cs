using System.Diagnostics.CodeAnalysis;
using System.Management.Automation;
using System.Management.Automation.Host;
using System.Runtime.Versioning;
using Microsoft.Win32;
using Moq;
using Xunit;

namespace WinPrefs.Tests {
    [ExcludeFromCodeCoverageAttribute]
    [SupportedOSPlatform("windows")]
    public class SavePreferencesTest {
        [Fact]
        public void Invoke_ShouldCreateOutputDirectory_WhenNotSpecified() {
            // Arrange
            var cmdlet = new SavePreferences();
            var mockLibPrefs = new Mock<LibPrefs>(new UnsafeHandleUtil());
            string expectedOutputDir = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
                "prefs-export");

            // Act
            cmdlet.ProcessInternal();

            // Assert
            Xunit.Assert.Equal(expectedOutputDir, cmdlet.OutputDirectory);
            Xunit.Assert.True(Directory.Exists(expectedOutputDir));
        }

        [Fact]
        public void Invoke_ShouldThrowError_WhenSavePreferencesFails() {
            // Arrange
            var cmdlet = new SavePreferences();
            var mockLibPrefs = new Mock<LibPrefs>(new UnsafeHandleUtil());
            mockLibPrefs.Setup(lp => lp.SavePreferences(It.IsAny<RegistryKey>(),
                                                        It.IsAny<LibPrefs.WriteObject>(),
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
            var psEmulator = new PowershellEmulator();
            cmdlet.CommandRuntime = psEmulator;
            cmdlet.prefs = mockLibPrefs.Object;

            // Act
            var exception = Xunit.Assert.Throws<InvalidOperationException>(
                () => cmdlet.ProcessInternal());
            Xunit.Assert.Equal("Failed to save.", exception.InnerException?.Message);
        }

        [Fact]
        public void Invoke_ShouldCallSavePreferences_WithCorrectParameters() {
            // Arrange
            var cmdlet = new SavePreferences {
                Commit = true,
                DeployKey = "deployKeyPath",
                Format = "cs",
                OutputDirectory = "outputDir",
                OutputFile = "-",
                Path = @"HKCU:\SomePath"
            };
            var mockLibPrefs = new Mock<LibPrefs>(new UnsafeHandleUtil());
            mockLibPrefs.Setup(lp => lp.SavePreferences(It.IsAny<RegistryKey>(),
                                                        It.IsAny<LibPrefs.WriteObject>(),
                                                        It.IsAny<bool>(),
                                                        It.IsAny<bool>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<int>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<LibPrefs.OutputFormat>()))
                                                        .Returns(true);
            var psEmulator = new PowershellEmulator();
            cmdlet.CommandRuntime = psEmulator;
            cmdlet.prefs = mockLibPrefs.Object;

            // Act
            cmdlet.ProcessInternal();

            // Assert
            mockLibPrefs.Verify(lp => lp.SavePreferences(It.IsAny<RegistryKey>(),
                                                        It.IsAny<LibPrefs.WriteObject>(),
                                                        true,
                                                        true,
                                                        "deployKeyPath",
                                                        "outputDir",
                                                        "-",
                                                        20,
                                                        @"HKCU:\SomePath",
                                                        LibPrefs.OutputFormat.CSharp), Times.Once);
        }
    }
}
