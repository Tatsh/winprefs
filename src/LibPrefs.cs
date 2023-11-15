using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;
using Microsoft.Win32;

namespace WinPrefsTestingArea
{
    [SupportedOSPlatform("windows")]
    public class LibPrefs
    {
        public enum OutputFormat
        {
            C,
            CSharp,
            PowerShell,
            Reg,
            Unknown
        }

        public static OutputFormat ToEnum(string format)
        {
            switch (format.ToLower())
            {
                case "ps":
                case "ps1":
                    return OutputFormat.PowerShell;

                case "cs":
                case "c#":
                    return OutputFormat.CSharp;

                case "c":
                    return OutputFormat.C;

                default:
                    return OutputFormat.Reg;
            }
        }

        private static IntPtr? ToUnsafeHandle(RegistryKey key)
        {
            Type registryKeyType = typeof(RegistryKey);
            FieldInfo? fieldInfo;
            SafeHandle? handle;
            try
            {
                fieldInfo = registryKeyType.GetField("_hkey", BindingFlags.NonPublic | BindingFlags.Instance);
            }
            catch (ArgumentNullException)
            {
                return null;
            }
            if (fieldInfo == null)
            {
                return null;
            }
            try
            {
                handle = (SafeHandle?)fieldInfo.GetValue(key);
            }
            catch (Exception)
            {
                return null;
            }
            if (handle == null)
            {
                return null;
            }
            return handle.DangerousGetHandle();
        }
        public static RegistryKey GetTopKey(string RegPath)
        {
            string firstPart = RegPath.Split(":").First().ToUpper();
            switch (firstPart)
            {
                case "HKCU":
                case "HKEY_CURRENT_USER":
                    return Registry.CurrentUser;

                case "HKCR":
                case "HKEY_CLASSES_ROOT":
                    return Registry.ClassesRoot;

                case "HKLM":
                case "HKEY_LOCAL_MACHINE":
                    return Registry.LocalMachine;

                case "HKCC":
                case "HKEY_CURRENT_CONFIG":
                    return Registry.CurrentConfig;

                default:
                    return Registry.Users;
            }
        }

        /**
         * Really, hk is required.
         */
        [DllImport("prefs.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.Cdecl, ThrowOnUnmappableChar = true, EntryPoint = "save_preferences")]
        private static extern bool SavePreferencesImpl(bool commit, string? deployKey, string? outputDirectory, string outputFile, int maxDepth, UIntPtr hk, string? specifiedPath, OutputFormat format);

        public unsafe static bool SavePreferences(RegistryKey hk, bool commit = false, string? deployKey = null, string? outputDirectory = null, string outputFile = "exec-reg.bat", int maxDepth = 20, string? specifiedPath = null, OutputFormat format = OutputFormat.Reg)
        {
            IntPtr? handle = ToUnsafeHandle(hk);
            if (handle == null)
            {
                return false;
            }
            return SavePreferencesImpl(commit, deployKey, outputDirectory, outputFile, maxDepth, (UIntPtr)handle.Value.ToPointer(), specifiedPath, format);
        }

        [DllImport("prefs.dll", CharSet = CharSet.Unicode, SetLastError = true, CallingConvention = CallingConvention.Cdecl, ThrowOnUnmappableChar = true, EntryPoint = "export_single_value")]
        private static extern bool ExportSingleValueImpl(UIntPtr topKey, string regPath, OutputFormat format = OutputFormat.Reg);

        public unsafe static bool ExportSingleValue(RegistryKey topKey, string regPath, OutputFormat format = OutputFormat.Reg)
        {
            IntPtr? handle = ToUnsafeHandle(topKey);
            if (handle == null)
            {
                return false;
            }
            return ExportSingleValueImpl((UIntPtr)handle.Value.ToPointer(), regPath, format);
        }

        [DllImport("prefs.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "set_debug_print_enabled")]
        public static extern void SetDebugPrintEnabled(bool enabled = true);
    }
}
