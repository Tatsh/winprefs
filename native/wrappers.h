#ifndef WRAPPERS_H
#define WRAPPERS_H

static inline DWORD wGetFileAttributes(wchar_t *path) {
    return GetFileAttributes(path);
}

static inline int wGetTimeFormatW(LCID Locale,
                                  DWORD dwFlags,
                                  const SYSTEMTIME *lpTime,
                                  LPCWSTR lpFormat,
                                  LPWSTR lpTimeStr,
                                  int cchTime) {
    return GetTimeFormatW(Locale, dwFlags, lpTime, lpFormat, lpTimeStr, cchTime);
}

static inline int wGetDateFormatW(LCID Locale,
                                  DWORD dwFlags,
                                  const SYSTEMTIME *lpDate,
                                  LPCWSTR lpFormat,
                                  LPWSTR lpDateStr,
                                  int cchDate) {
    return GetDateFormatW(Locale, dwFlags, lpDate, lpFormat, lpDateStr, cchDate);
}

static inline BOOL wCreatePipe(PHANDLE hReadPipe,
                               PHANDLE hWritePipe,
                               LPSECURITY_ATTRIBUTES lpPipeAttributes,
                               DWORD nSize) {
    return CreatePipe(hReadPipe, hWritePipe, lpPipeAttributes, nSize);
}

static inline BOOL wCloseHandle(HANDLE hObject) {
    return CloseHandle(hObject);
}

static inline DWORD wWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds) {
    return WaitForSingleObject(hHandle, dwMilliseconds);
}

static inline BOOL wPeekNamedPipe(HANDLE hNamedPipe,
                                  LPVOID lpBuffer,
                                  DWORD nBufferSize,
                                  LPDWORD lpBytesRead,
                                  LPDWORD lpTotalBytesAvail,
                                  LPDWORD lpBytesLeftThisMessage) {
    return PeekNamedPipe(
        hNamedPipe, lpBuffer, nBufferSize, lpBytesRead, lpTotalBytesAvail, lpBytesLeftThisMessage);
}

static inline BOOL wReadFile(HANDLE hFile,
                             LPVOID lpBuffer,
                             DWORD nNumberOfBytesToRead,
                             LPDWORD lpNumberOfBytesRead,
                             LPOVERLAPPED lpOverlapped) {
    return ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

static inline int wMultiByteToWideChar(UINT CodePage,
                                       DWORD dwFlags,
                                       LPCCH lpMultiByteStr,
                                       int cbMultiByte,
                                       LPWSTR lpWideCharStr,
                                       int cchWideChar) {
    return MultiByteToWideChar(
        CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
}

static inline int wWideCharToMultiByte(UINT CodePage,
                                       DWORD dwFlags,
                                       LPCWCH lpWideCharStr,
                                       int cchWideChar,
                                       LPSTR lpMultiByteStr,
                                       int cbMultiByte,
                                       LPCCH lpDefaultChar,
                                       LPBOOL lpUsedDefaultChar) {
    return WideCharToMultiByte(CodePage,
                               dwFlags,
                               lpWideCharStr,
                               cchWideChar,
                               lpMultiByteStr,
                               cbMultiByte,
                               lpDefaultChar,
                               lpUsedDefaultChar);
}

static inline void wPathStripPathW(LPWSTR pszPath) {
    PathStripPathW(pszPath);
}

static inline BOOL wCreateProcessW(LPCWSTR lpApplicationName,
                                   LPWSTR lpCommandLine,
                                   LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                   LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                   BOOL bInheritHandles,
                                   DWORD dwCreationFlags,
                                   LPVOID lpEnvironment,
                                   LPCWSTR lpCurrentDirectory,
                                   LPSTARTUPINFOW lpStartupInfo,
                                   LPPROCESS_INFORMATION lpProcessInformation) {
    return CreateProcessW(lpApplicationName,
                          lpCommandLine,
                          lpProcessAttributes,
                          lpThreadAttributes,
                          bInheritHandles,
                          dwCreationFlags,
                          lpEnvironment,
                          lpCurrentDirectory,
                          lpStartupInfo,
                          lpProcessInformation);
}

static inline BOOL wWriteFile(HANDLE hFile,
                              LPCVOID lpBuffer,
                              DWORD nNumberOfBytesToWrite,
                              LPDWORD lpNumberOfBytesWritten,
                              LPOVERLAPPED lpOverlapped) {
    return WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

static inline LSTATUS wRegEnumValueW(HKEY hKey,
                                     DWORD dwIndex,
                                     LPWSTR lpValueName,
                                     LPDWORD lpcchValueName,
                                     LPDWORD lpReserved,
                                     LPDWORD lpType,
                                     LPBYTE lpData,
                                     LPDWORD lpcbData) {
    return RegEnumValueW(
        hKey, dwIndex, lpValueName, lpcchValueName, lpReserved, lpType, lpData, lpcbData);
}

static inline DWORD wGetLastError() {
    return GetLastError();
}

static inline BOOL wCreateDirectoryW(LPCWSTR lpPathName,
                                     LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
    return CreateDirectoryW(lpPathName, lpSecurityAttributes);
}

static inline BOOL wPathAppendW(LPWSTR pszPath, LPCWSTR pszMore) {
    return PathAppendW(pszPath, pszMore);
}

static inline HANDLE wGetStdHandle(DWORD nStdHandle) {
    return GetStdHandle(nStdHandle);
}

static inline LSTATUS
wRegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    return RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

static inline LSTATUS wRegEnumKeyExW(HKEY hKey,
                                     DWORD dwIndex,
                                     LPWSTR lpName,
                                     LPDWORD lpcchName,
                                     LPDWORD lpReserved,
                                     LPWSTR lpClass,
                                     LPDWORD lpcchClass,
                                     PFILETIME lpftLastWriteTime) {
    return RegEnumKeyExW(
        hKey, dwIndex, lpName, lpcchName, lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
}

static inline LSTATUS wRegCloseKey(HKEY hKey) {
    return RegCloseKey(hKey);
}

static inline HANDLE wCreateFileW(LPCWSTR lpFileName,
                                  DWORD dwDesiredAccess,
                                  DWORD dwShareMode,
                                  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                  DWORD dwCreationDisposition,
                                  DWORD dwFlagsAndAttributes,
                                  HANDLE hTemplateFile) {
    return CreateFileW(lpFileName,
                       dwDesiredAccess,
                       dwShareMode,
                       lpSecurityAttributes,
                       dwCreationDisposition,
                       dwFlagsAndAttributes,
                       hTemplateFile);
}

static inline LSTATUS wRegQueryInfoKeyW(HKEY hKey,
                                        LPWSTR lpClass,
                                        LPDWORD lpcchClass,
                                        LPDWORD lpReserved,
                                        LPDWORD lpcSubKeys,
                                        LPDWORD lpcbMaxSubKeyLen,
                                        LPDWORD lpcbMaxClassLen,
                                        LPDWORD lpcValues,
                                        LPDWORD lpcbMaxValueNameLen,
                                        LPDWORD lpcbMaxValueLen,
                                        LPDWORD lpcbSecurityDescriptor,
                                        PFILETIME lpftLastWriteTime) {
    return RegQueryInfoKeyW(hKey,
                            lpClass,
                            lpcchClass,
                            lpReserved,
                            lpcSubKeys,
                            lpcbMaxSubKeyLen,
                            lpcbMaxClassLen,
                            lpcValues,
                            lpcbMaxValueNameLen,
                            lpcbMaxValueLen,
                            lpcbSecurityDescriptor,
                            lpftLastWriteTime);
}

static inline LSTATUS wRegQueryValueExW(HKEY hKey,
                                        LPCWSTR lpValueName,
                                        LPDWORD lpReserved,
                                        LPDWORD lpType,
                                        LPBYTE lpData,
                                        LPDWORD lpcbData) {
    return RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

static inline HRESULT
wSHGetFolderPathW(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath) {
    return SHGetFolderPathW(hwnd, csidl, hToken, dwFlags, pszPath);
}

static inline DWORD wFormatMessageW(DWORD dwFlags,
                                    LPCVOID lpSource,
                                    DWORD dwMessageId,
                                    DWORD dwLanguageId,
                                    LPWSTR lpBuffer,
                                    DWORD nSize,
                                    va_list *Arguments) {
    return FormatMessageW(dwFlags, lpSource, dwMessageId, dwLanguageId, lpBuffer, nSize, Arguments);
}

#endif // WRAPPERS_H
