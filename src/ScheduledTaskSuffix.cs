using System.Security.Cryptography;
using System.Text;

namespace WinPrefs {
    public class ScheduledTaskSuffix {
        public string Get(bool commit, string? deployKey, string? outputDirectory, string outputFile,
            string path, string format, int maxDepth) {
            using var sha1 = SHA1.Create();
            return Convert.ToHexString(sha1.ComputeHash(
                Encoding.UTF8.GetBytes(
               $"c={commit},K={deployKey},o={outputDirectory},f={outputFile},p={path}," +
               $"f={format},m={maxDepth}")));
        }
    }
}
