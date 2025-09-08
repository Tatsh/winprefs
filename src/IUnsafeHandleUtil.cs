using Microsoft.Win32;

namespace WinPrefs {
    public interface IUnsafeHandleUtil {
        public IntPtr? ToUnsafeHandle(RegistryKey key);
    }
}
