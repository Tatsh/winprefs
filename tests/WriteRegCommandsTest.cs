using System;
using System.IO;
using System.Management.Automation;
using System.Runtime.Versioning;
using System.Collections.Generic;
using Microsoft.Win32;
using Moq;
using Xunit;

namespace WinPrefs.Tests {
    [SupportedOSPlatform("windows")]
    public class WriteRegCommandsTest {
        [Fact]
        public void Invoke_ShouldThrowError_WhenRegistryPathIsInvalid() {
            // Arrange
            var cmdlet = new WriteRegCommands {
                Path = "HKCU:\\Software2\\Invalid"
            };
            var mockLibPrefs = new Mock<LibPrefs>();
            mockLibPrefs.Setup(p => p.ExportSingleValue(It.IsAny<RegistryKey>(),
                                                          It.IsAny<string>(),
                                                          It.IsAny<LibPrefs.WriteObject>(),
                                                          It.IsAny<LibPrefs.OutputFormat>())).Returns(false);
            cmdlet.prefs = mockLibPrefs.Object;

            // Act & Assert
            var exception = Assert.Throws<Exception>(() => cmdlet.ProcessInternal());
            Assert.Equal("Failed to export HKCU:\\Software2\\Invalid as a single value.", exception.Message);
        }

        [Fact]
        public void Invoke_ShouldCallSavePreferences_WithCorrectParameters() {
            // Arrange
            var cmdlet = new WriteRegCommands {
                Path = "HKCU:\\Software",
                Format = "cs",
                MaxDepth = 10
            };
            var mockLibPrefs = new Mock<LibPrefs>();
            mockLibPrefs.Setup(lp => lp.SavePreferences(It.IsAny<RegistryKey>(),
                                                        It.IsAny<LibPrefs.WriteObject>(),
                                                        It.IsAny<bool>(),
                                                        It.IsAny<bool>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<int>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<LibPrefs.OutputFormat>())).Returns(true);
            cmdlet.prefs = mockLibPrefs.Object;

            // Act
            cmdlet.ProcessInternal();

            // Assert
            mockLibPrefs.Verify(lp => lp.SavePreferences(It.IsAny<RegistryKey>(),
                                                         It.IsAny<LibPrefs.WriteObject>(),
                                                         true,
                                                         false,
                                                         null,
                                                         null,
                                                         "-",
                                                         10,
                                                         "HKCU:\\Software",
                                                         LibPrefs.OutputFormat.CSharp));
        }

        [Fact]
        public void Invoke_ShouldThrowError_WhenSavePreferencesFails() {
            // Arrange
            var cmdlet = new WriteRegCommands {
                Path = "HKCU:\\Software"
            };
            var mockLibPrefs = new Mock<LibPrefs>();
            mockLibPrefs.Setup(lp => lp.SavePreferences(It.IsAny<RegistryKey>(),
                                                        It.IsAny<LibPrefs.WriteObject>(),
                                                        It.IsAny<bool>(),
                                                        It.IsAny<bool>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<int>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<LibPrefs.OutputFormat>())).Returns(false);
            cmdlet.prefs = mockLibPrefs.Object;

            // Act & Assert
            var exception = Assert.Throws<Exception>(() => cmdlet.ProcessInternal());
            Assert.Equal("Failed to export HKCU:\\Software.", exception.Message);
        }

        [Fact]
        public void Invoke_ShouldCallExportSingleValue_WhenSubKeyIsNull() {
            // Arrange
            var cmdlet = new WriteRegCommands {
                Path = "HKCU:\\Software\\NonExistentSubKey",
                Format = "cs"
            };
            var mockLibPrefs = new Mock<LibPrefs>();
            mockLibPrefs.Setup(lp => lp.ExportSingleValue(It.IsAny<RegistryKey>(),
                                                          It.IsAny<string>(),
                                                          It.IsAny<LibPrefs.WriteObject>(),
                                                          It.IsAny<LibPrefs.OutputFormat>())).Returns(true);
            cmdlet.prefs = mockLibPrefs.Object;

            // Act
            cmdlet.ProcessInternal();

            // Assert
            mockLibPrefs.Verify(lp => lp.ExportSingleValue(It.IsAny<RegistryKey>(),
                                                           "HKCU:\\Software\\NonExistentSubKey",
                                                           It.IsAny<LibPrefs.WriteObject>(),
                                                           LibPrefs.OutputFormat.CSharp), Times.Once);
        }

        [Fact]
        public void Invoke_ShouldThrowError_WhenExportSingleValueFails() {
            // Arrange
            var cmdlet = new WriteRegCommands {
                Path = "HKCU:\\Software\\NonExistentSubKey",
                Format = "cs"
            };
            var mockLibPrefs = new Mock<LibPrefs>();
            mockLibPrefs.Setup(lp => lp.ExportSingleValue(It.IsAny<RegistryKey>(),
                                                          It.IsAny<string>(),
                                                          It.IsAny<LibPrefs.WriteObject>(),
                                                          It.IsAny<LibPrefs.OutputFormat>())).Returns(false);

            // Act & Assert
            var exception = Assert.Throws<Exception>(() => cmdlet.ProcessInternal());
            Assert.Equal("Failed to export HKCU:\\Software\\NonExistentSubKey as a single value.", exception.Message);
        }
    }
}
