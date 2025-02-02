// FILE: src/LibPrefsTest.cs

using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;
using Microsoft.Win32;
using Moq;
using Xunit;

namespace WinPrefs.Tests {
    [SupportedOSPlatform("windows")]
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
    }
}
