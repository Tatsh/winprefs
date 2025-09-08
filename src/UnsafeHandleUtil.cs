using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using System.Runtime.InteropServices;
using Microsoft.Win32;

namespace WinPrefs {
    [ExcludeFromCodeCoverageAttribute]
    class UnsafeHandleUtil : IUnsafeHandleUtil {
        virtual public IntPtr? ToUnsafeHandle(RegistryKey key) {
            FieldInfo? fieldInfo;
            SafeHandle? handle;
            Type registryKeyType = typeof(RegistryKey);
            try {
                fieldInfo = registryKeyType.GetField("_hkey",
                    BindingFlags.NonPublic | BindingFlags.Instance);
            } catch (ArgumentNullException) {
                return null;
            }
            try {
                handle = (SafeHandle?)fieldInfo?.GetValue(key);
            } catch (Exception) {
                return null;
            }
            return handle?.DangerousGetHandle();
        }
    }
}
