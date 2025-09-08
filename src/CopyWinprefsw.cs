using System.Diagnostics.CodeAnalysis;
using System.Linq.Expressions;
using IOPath = System.IO.Path;

namespace WinPrefs {
    [ExcludeFromCodeCoverageAttribute]
    public class CopyWinprefsw : ICopyWinprefsw {
        public string Copy(string executingAssemblyLocation, Action<Exception> errorCallback) {
            string appDataDir = IOPath.Combine(Environment.GetFolderPath(
                  Environment.SpecialFolder.LocalApplicationData), "WinPrefs");
            string winprefswPath = IOPath.Combine(appDataDir, "winprefsw.exe");
            Directory.CreateDirectory(appDataDir);
            if (File.Exists(winprefswPath)) {
                File.Delete(winprefswPath);
            }
            string? assemblyLoc = IOPath.GetDirectoryName(executingAssemblyLocation);
            if (assemblyLoc == null) {
                errorCallback(new Exception("Failed to get assembly location."));
                return "";
            }
            File.Copy(IOPath.Combine(assemblyLoc, "winprefsw.exe"), winprefswPath);
            return winprefswPath;
        }
    }
}
