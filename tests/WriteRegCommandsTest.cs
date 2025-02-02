using System;
using System.IO;
using System.Management.Automation;
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
                Path = "InvalidPath"
            };
            var mockLibPrefs = new Mock<LibPrefs>();

            // Act & Assert
            var exception = Assert.Throws<CmdletInvocationException>(() => cmdlet.Invoke());
            Assert.Equal("Failed to export InvalidPath as a single value.", exception.Message);
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
            mockLibPrefs.Setup(lp => lp.SavePreferences(It.IsAny<LibPrefs.OutputFormat>(),
                                                        It.IsAny<RegistryKey>(),
                                                        It.IsAny<int>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<Action<object>>(),
                                                        It.IsAny<bool>())).Returns(true);

            // Act
            cmdlet.Invoke();

            // Assert
            mockLibPrefs.Verify(lp => lp.SavePreferences(LibPrefs.OutputFormat.CSharp,
                                                         It.IsAny<RegistryKey>(),
                                                         10,
                                                         "-",
                                                         "HKCU:\\Software",
                                                         It.IsAny<Action<object>>(),
                                                         true), Times.Once);
        }

        [Fact]
        public void Invoke_ShouldThrowError_WhenSavePreferencesFails() {
            // Arrange
            var cmdlet = new WriteRegCommands {
                Path = "HKCU:\\Software"
            };
            var mockLibPrefs = new Mock<LibPrefs>();
            mockLibPrefs.Setup(lp => lp.SavePreferences(It.IsAny<LibPrefs.OutputFormat>(),
                                                        It.IsAny<RegistryKey>(),
                                                        It.IsAny<int>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<string>(),
                                                        It.IsAny<Action<object>>(),
                                                        It.IsAny<bool>())).Returns(false);

            // Act & Assert
            var exception = Assert.Throws<CmdletInvocationException>(() => cmdlet.Invoke());
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
                                                          It.IsAny<Action<object>>(),
                                                          It.IsAny<LibPrefs.OutputFormat>())).Returns(true);

            // Act
            cmdlet.Invoke();

            // Assert
            mockLibPrefs.Verify(lp => lp.ExportSingleValue(It.IsAny<RegistryKey>(),
                                                           "HKCU:\\Software\\NonExistentSubKey",
                                                           It.IsAny<Action<object>>(),
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
                                                          It.IsAny<Action<object>>(),
                                                          It.IsAny<LibPrefs.OutputFormat>())).Returns(false);

            // Act & Assert
            var exception = Assert.Throws<CmdletInvocationException>(() => cmdlet.Invoke());
            Assert.Equal("Failed to export HKCU:\\Software\\NonExistentSubKey as a single value.", exception.Message);
        }
    }
}
