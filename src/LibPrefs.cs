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
        public delegate bool WriterWriteOutputT(object instance,
                                                string mbOut,
                                                int totalSize,
                                                out uint written);
        [StructLayout(LayoutKind.Sequential)]
        public struct Writer {
            [MarshalAs(UnmanagedType.FunctionPtr)] public WriterSetupT? setup;
            [MarshalAs(UnmanagedType.FunctionPtr)] public WriterTeardownT? teardown;
            [MarshalAs(UnmanagedType.FunctionPtr)] public WriterWriteOutputT write;
        }

        internal delegate void WriteObject(object sendToPipeline);

        private static WriteObject? WriteObjectImpl;

        public static OutputFormat ToEnum(string format) {
            return format.ToLower() switch {
                "c" => OutputFormat.C,
                "cs" or "c#" => OutputFormat.CSharp,
                "powershell" or "ps" or "ps1" => OutputFormat.PowerShell,
                _ => OutputFormat.Reg,
            };
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
            return RegPath.Split(":").First().ToUpper() switch {
                "HKCC" or "HKEY_CURRENT_CONFIG" => Registry.CurrentConfig,
                "HKCR" or "HKEY_CLASSES_ROOT" => Registry.ClassesRoot,
                "HKCU" or "HKEY_CURRENT_USER" => Registry.CurrentUser,
                "HKLM" or "HKEY_LOCAL_MACHINE" => Registry.LocalMachine,
                _ => Registry.Users,
            };
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

        internal static bool WriteOutputImpl(object instance,
                                           string mbOut,
                                           int totalSize,
                                           out uint written) {
            WriteObjectImpl?.Invoke(mbOut[..(totalSize - 1)]);
            written = (uint)totalSize;
            return true;
        }

        internal unsafe static bool SavePreferences(RegistryKey hk,
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
            Writer writer = new();
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

        internal unsafe static bool ExportSingleValue(RegistryKey topKey,
                                                    string regPath,
                                                    WriteObject writeObjectIn,
                                                    OutputFormat format = OutputFormat.Reg) {
            WriteObjectImpl = writeObjectIn;
            IntPtr? handle = ToUnsafeHandle(topKey);
            if (handle == null) {
                return false;
            }
            Writer writer = new() {
                write = WriteOutputImpl
            };
            return ExportSingleValueImpl((UIntPtr)handle.Value.ToPointer(),
                                                          regPath, format, ref writer);
        }

        [DllImport("prefs.dll",
                   CallingConvention = CallingConvention.Cdecl,
                   EntryPoint = "set_debug_print_enabled")]
        internal static extern void SetDebugPrintEnabled(bool enabled = true);
    }
}
