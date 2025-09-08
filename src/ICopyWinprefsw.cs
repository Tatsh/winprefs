namespace WinPrefs {
    public interface ICopyWinprefsw {
        public string Copy(string executingAssemblyLocation, Action<Exception> errorCallback);
    }
}
