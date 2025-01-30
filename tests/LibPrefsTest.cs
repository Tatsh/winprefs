// FILE: src/LibPrefsTest.cs

using System;
using System.Reflection;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using Moq;
using Xunit;

namespace WinPrefs.Tests {
    public class LibPrefsTest {
        [Fact]
        public void ToEnum_ShouldReturnCorrectEnumValue() {
            Assert.Equal(LibPrefs.OutputFormat.C, LibPrefs.ToEnum("c"));
            Assert.Equal(LibPrefs.OutputFormat.CSharp, LibPrefs.ToEnum("cs"));
            Assert.Equal(LibPrefs.OutputFormat.PowerShell, LibPrefs.ToEnum("powershell"));
            Assert.Equal(LibPrefs.OutputFormat.Reg, LibPrefs.ToEnum("unknown"));
        }

        [Fact]
        public void GetTopKey_ShouldReturnCorrectRegistryKey() {
            Assert.Equal(Registry.CurrentConfig, LibPrefs.GetTopKey("HKCC:SomePath"));
            Assert.Equal(Registry.ClassesRoot, LibPrefs.GetTopKey("HKCR:SomePath"));
            Assert.Equal(Registry.CurrentUser, LibPrefs.GetTopKey("HKCU:SomePath"));
            Assert.Equal(Registry.LocalMachine, LibPrefs.GetTopKey("HKLM:SomePath"));
            Assert.Equal(Registry.Users, LibPrefs.GetTopKey("HKU:SomePath"));
        }

        [Fact]
        public void ToUnsafeHandle_ShouldReturnCorrectHandle() {
            using RegistryKey key = Registry.CurrentUser.OpenSubKey("Software");
            IntPtr? handle = LibPrefs.ToUnsafeHandle(key);
            Assert.NotNull(handle);
        }

        [Fact]
        public void SavePreferences_ShouldReturnFalse_WhenHandleIsNull() {
            var result = LibPrefs.SavePreferences(null, _ => { });
            Assert.False(result);
        }

        [Fact]
        public void SavePreferences_ShouldInvokeSavePreferencesImpl() {
            var mock = new Mock<LibPrefs.WriterWriteOutputT>();
            mock.Setup(m => m(It.IsAny<object>(),
                              It.IsAny<string>(),
                              It.IsAny<int>(),
                              out It.Ref<uint>.IsAny)).Returns(true);
            var writer = new LibPrefs.Writer {
                write = mock.Object
            };
            var result = LibPrefs.SavePreferences(Registry.CurrentUser, _ => { }, true);
            Assert.True(result);
        }

        [Fact]
        public void ExportSingleValue_ShouldReturnFalse_WhenHandleIsNull() {
            var result = LibPrefs.ExportSingleValue(null, "SomePath", _ => { });
            Assert.False(result);
        }

        [Fact]
        public void ExportSingleValue_ShouldInvokeExportSingleValueImpl() {
            var mock = new Mock<LibPrefs.WriterWriteOutputT>();
            mock.Setup(m => m(It.IsAny<object>(),
                              It.IsAny<string>(),
                              It.IsAny<int>(),
                              out It.Ref<uint>.IsAny)).Returns(true);
            var writer = new LibPrefs.Writer {
                write = mock.Object
            };
            var result = LibPrefs.ExportSingleValue(Registry.CurrentUser, "SomePath", _ => { });
            Assert.True(result);
        }

        [Fact]
        public void SetDebugPrintEnabled_ShouldInvokeNativeMethod() {
            LibPrefs.SetDebugPrintEnabled(true);
        }
    }
}
