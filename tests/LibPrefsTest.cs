using System.Diagnostics.CodeAnalysis;
using System.Runtime.Versioning;
using Microsoft.Win32;
using Moq;
using Xunit;

namespace WinPrefs.Tests {
    [ExcludeFromCodeCoverageAttribute]
    [SupportedOSPlatform("windows")]
    public class LibPrefsTest {
        [Fact]
        public void ToEnum_ShouldReturnCorrectEnumValue() {
            Xunit.Assert.Equal(LibPrefs.OutputFormat.C, LibPrefs.ToEnum("c"));
            Xunit.Assert.Equal(LibPrefs.OutputFormat.CSharp, LibPrefs.ToEnum("cs"));
            Xunit.Assert.Equal(LibPrefs.OutputFormat.PowerShell, LibPrefs.ToEnum("powershell"));
            Xunit.Assert.Equal(LibPrefs.OutputFormat.Reg, LibPrefs.ToEnum("unknown"));
        }

        [Fact]
        public void GetTopKey_ShouldReturnCorrectRegistryKey() {
            Xunit.Assert.Equal(Registry.CurrentConfig, LibPrefs.GetTopKey("HKCC:SomePath"));
            Xunit.Assert.Equal(Registry.ClassesRoot, LibPrefs.GetTopKey("HKCR:SomePath"));
            Xunit.Assert.Equal(Registry.CurrentUser, LibPrefs.GetTopKey("HKCU:SomePath"));
            Xunit.Assert.Equal(Registry.LocalMachine, LibPrefs.GetTopKey("HKLM:SomePath"));
            Xunit.Assert.Equal(Registry.Users, LibPrefs.GetTopKey("HKU:SomePath"));
        }

        [Fact]
        public void ExportSingleValue_ReturnsFalseIfHandleIsNull() {
            var mockUnsafeUtil = new Mock<IUnsafeHandleUtil>();
            LibPrefs lp = new(mockUnsafeUtil.Object);
            mockUnsafeUtil.Setup(
                mockUnsafeUtil => mockUnsafeUtil.ToUnsafeHandle(It.IsAny<RegistryKey>())).Returns(
                    (nint?)null);
            bool result = lp.ExportSingleValue(Registry.CurrentUser, "SomePath",
                new SavePreferences().WriteObject);
            Xunit.Assert.False(result);
        }

        [Fact]
        public void SavePreferences_ReturnsFalseIfHandleIsNull() {
            var mockUnsafeUtil = new Mock<IUnsafeHandleUtil>();
            LibPrefs lp = new(mockUnsafeUtil.Object);
            mockUnsafeUtil.Setup(
                mockUnsafeUtil => mockUnsafeUtil.ToUnsafeHandle(It.IsAny<RegistryKey>())).Returns(
                    (nint?)null);
            bool result = lp.SavePreferences(Registry.CurrentUser,
                new SavePreferences().WriteObject);
            Xunit.Assert.False(result);
        }
    }
}
