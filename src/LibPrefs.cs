using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;

using Microsoft.Win32;

namespace WinPrefs {
    [SupportedOSPlatform("windows")]
    public class LibPrefs {
        public enum OutputFormat {
            C,
            CSharp,
            PowerShell,
            Reg,
            Unknown
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate bool WriterSetupT();
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void WriterTeardownT();
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate bool WriterWriteOutputT(object instance, string mbOut, int totalSize, out uint written);
        [StructLayout(LayoutKind.Sequential)]
        public struct Writer {
            [MarshalAs(UnmanagedType.FunctionPtr)] public WriterSetupT? setup;
            [MarshalAs(UnmanagedType.FunctionPtr)] public WriterTeardownT? teardown;
            [MarshalAs(UnmanagedType.FunctionPtr)] public WriterWriteOutputT write;
        }

        public delegate void WriteObject(object sendToPipeline);

        public static WriteObject? WriteObjectImpl;

        public static OutputFormat ToEnum(string format) {
            switch (format.ToLower()) {
                case "c":
                    return OutputFormat.C;
                case "cs":
                case "c#":
                    return OutputFormat.CSharp;
                case "powershell":
                case "ps":
                case "ps1":
                    return OutputFormat.PowerShell;
                default:
                    return OutputFormat.Reg;
            }
        }

        private static IntPtr? ToUnsafeHandle(RegistryKey key) {
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

        public static RegistryKey GetTopKey(string RegPath) {
            switch (RegPath.Split(":").First().ToUpper()) {
                case "HKCC":
                case "HKEY_CURRENT_CONFIG":
                    return Registry.CurrentConfig;
                case "HKCR":
                case "HKEY_CLASSES_ROOT":
                    return Registry.ClassesRoot;
                case "HKCU":
                case "HKEY_CURRENT_USER":
                    return Registry.CurrentUser;
                case "HKLM":
                case "HKEY_LOCAL_MACHINE":
                    return Registry.LocalMachine;
                default:
                    return Registry.Users;
            }
        }

        [DllImport("prefs.dll",
                   CallingConvention = CallingConvention.Cdecl,
                   CharSet = CharSet.Unicode,
                   EntryPoint = "save_preferences",
                   SetLastError = true,
                   ThrowOnUnmappableChar = true)]
        private static extern bool SavePreferencesImpl(bool commit,
                                                       string? deployKey,
                                                       string? outputDirectory,
                                                       string outputFile,
                                                       int maxDepth,
                                                       UIntPtr hk,
                                                       string? specifiedPath,
                                                       OutputFormat format,
                                                       ref Writer writer);

        public static bool WriteOutputImpl(object instance, string mbOut, int totalSize, out uint written) {
            if (WriteObjectImpl != null) {
                WriteObjectImpl(mbOut.Substring(0, totalSize - 1));
            }
            written = (uint)totalSize;
            return true;
        }

        public unsafe static bool SavePreferences(RegistryKey hk,
            WriteObject writeObjectIn,
                                                  bool writeStdOut = false,
                                                  bool commit = false,
                                                  string? deployKey = null,
                                                  string? outputDirectory = null,
                                                  string outputFile = "exec-reg.bat",
                                                  int maxDepth = 20,
                                                  string? specifiedPath = null,
                                                  OutputFormat format = OutputFormat.Reg) {
            WriteObjectImpl = writeObjectIn;
            IntPtr? handle = ToUnsafeHandle(hk);
            if (handle == null) {
                return false;
            }
            Writer writer = new Writer();
            if (writeStdOut) {
                writer.write = WriteOutputImpl;
            }
            return SavePreferencesImpl(commit, deployKey, outputDirectory,
                                                        outputFile, maxDepth,
                                                        (UIntPtr)handle.Value.ToPointer(),
                                                        specifiedPath, format, ref writer);
        }

        [DllImport("prefs.dll",
                   CallingConvention = CallingConvention.Cdecl,
                   CharSet = CharSet.Unicode,
                   EntryPoint = "export_single_value",
                   SetLastError = true,
                   ThrowOnUnmappableChar = true)]
        private static extern bool ExportSingleValueImpl(UIntPtr topKey,
                                                         string regPath,
                                                         OutputFormat format,
                                                         ref Writer writer);

        public unsafe static bool ExportSingleValue(RegistryKey topKey,
                                                    string regPath,
                                                    WriteObject writeObjectIn,
                                                    OutputFormat format = OutputFormat.Reg) {
            WriteObjectImpl = writeObjectIn;
            IntPtr? handle = ToUnsafeHandle(topKey);
            if (handle == null) {
                return false;
            }
            Writer writer = new Writer();
            writer.write = WriteOutputImpl;
            return ExportSingleValueImpl((UIntPtr)handle.Value.ToPointer(),
                                                          regPath, format, ref writer);
        }

        [DllImport("prefs.dll",
                   CallingConvention = CallingConvention.Cdecl,
                   EntryPoint = "set_debug_print_enabled")]
        public static extern void SetDebugPrintEnabled(bool enabled = true);
    }
}
