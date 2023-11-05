#include "wrappers.h"

LSTATUS __wrap_wRegCloseKey(HKEY hKey) {
    return mock_type(LSTATUS);
}

intptr_t __wrap__wspawnlp(int mode, const wchar_t *cmd, const wchar_t *arg_list, ...) {
    return mock_type(intptr_t);
}

int __wrap_wGetTimeFormatW(LCID Locale,
                           DWORD dwFlags,
                           const SYSTEMTIME *lpTime,
                           LPCWSTR lpFormat,
                           LPWSTR lpTimeStr,
                           int cchTime) {
    return mock_type(int);
}

int __wrap_wGetDateFormatW(LCID Locale,
                           DWORD dwFlags,
                           const SYSTEMTIME *lpDate,
                           LPCWSTR lpFormat,
                           LPWSTR lpDateStr,
                           int cchDate) {
    return mock_type(int);
}

wchar_t *__wrap__wfullpath(wchar_t *absPath, const wchar_t *relPath, size_t maxLength) {
    wchar_t *absPathRet = mock_ptr_type(wchar_t *);
    wmemset(absPath, L'\0', maxLength);
    if (absPathRet) {
        wmemcpy(absPath, absPathRet, wcslen(absPathRet));
    }
    return mock_ptr_type(wchar_t *);
}

DWORD __wrap_wGetLastError() {
    return mock_type(DWORD);
}

int __wrap__wchdir(const wchar_t *dirname) {
    return mock_type(int);
}

BOOL __wrap_wCloseHandle(HANDLE hObject) {
    return mock_type(BOOL);
}

BOOL __wrap_wCreateProcessW(LPCWSTR lpApplicationName,
                            LPWSTR lpCommandLine,
                            LPSECURITY_ATTRIBUTES lpProcessAttributes,
                            LPSECURITY_ATTRIBUTES lpThreadAttributes,
                            BOOL bInheritHandles,
                            DWORD dwCreationFlags,
                            LPVOID lpEnvironment,
                            LPCWSTR lpCurrentDirectory,
                            LPSTARTUPINFOW lpStartupInfo,
                            LPPROCESS_INFORMATION lpProcessInformation) {
    return mock_type(BOOL);
}

BOOL __wrap_wWriteFile(HANDLE hFile,
                       LPCVOID lpBuffer,
                       DWORD nNumberOfBytesToWrite,
                       LPDWORD lpNumberOfBytesWritten,
                       LPOVERLAPPED lpOverlapped) {
    return mock_type(BOOL);
}

HANDLE __wrap_wGetStdHandle(DWORD nStdHandle) {
    return mock_ptr_type(HANDLE);
}

int __wrap_wMultiByteToWideChar(UINT CodePage,
                                DWORD dwFlags,
                                LPCCH lpMultiByteStr,
                                int cbMultiByte,
                                LPWSTR lpWideCharStr,
                                int cchWideChar) {
    return mock_type(int);
}

int __wrap_wWideCharToMultiByte(UINT CodePage,
                                DWORD dwFlags,
                                LPCWCH lpWideCharStr,
                                int cchWideChar,
                                LPSTR lpMultiByteStr,
                                int cbMultiByte,
                                LPCCH lpDefaultChar,
                                LPBOOL lpUsedDefaultChar) {
    return mock_type(int);
}

void __wrap_wPathStripPathW(LPWSTR pszPath) {
}

LSTATUS __wrap_wRegEnumValueW(HKEY hKey,
                              DWORD dwIndex,
                              LPWSTR lpValueName,
                              LPDWORD lpcchValueName,
                              LPDWORD lpReserved,
                              LPDWORD lpType,
                              LPBYTE lpData,
                              LPDWORD lpcbData) {
    return mock_type(LSTATUS);
}

DWORD __wrap_wWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds) {
    return mock_type(DWORD);
}

BOOL __wrap_wPeekNamedPipe(HANDLE hNamedPipe,
                           LPVOID lpBuffer,
                           DWORD nBufferSize,
                           LPDWORD lpBytesRead,
                           LPDWORD lpTotalBytesAvail,
                           LPDWORD lpBytesLeftThisMessage) {
    return mock_type(BOOL);
}

BOOL __wrap_wReadFile(HANDLE hFile,
                      LPVOID lpBuffer,
                      DWORD nNumberOfBytesToRead,
                      LPDWORD lpNumberOfBytesRead,
                      LPOVERLAPPED lpOverlapped) {
    return mock_type(BOOL);
}

HANDLE __wrap_wCreateFileW(LPCWSTR lpFileName,
                           DWORD dwDesiredAccess,
                           DWORD dwShareMode,
                           LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                           DWORD dwCreationDisposition,
                           DWORD dwFlagsAndAttributes,
                           HANDLE hTemplateFile) {
    return mock_ptr_type(HANDLE);
}

LSTATUS __wrap_wRegEnumKeyExW(HKEY hKey,
                              DWORD dwIndex,
                              LPWSTR lpName,
                              LPDWORD lpcchName,
                              LPDWORD lpReserved,
                              LPWSTR lpClass,
                              LPDWORD lpcchClass,
                              PFILETIME lpftLastWriteTime) {
    return mock_type(LSTATUS);
}

LSTATUS __wrap_wRegQueryInfoKeyW(HKEY hKey,
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
    return mock_type(LSTATUS);
}

LSTATUS __wrap_wRegQueryValueExW(HKEY hKey,
                                 LPCWSTR lpValueName,
                                 LPDWORD lpReserved,
                                 LPDWORD lpType,
                                 LPBYTE lpData,
                                 LPDWORD lpcbData) {
    return mock_type(LSTATUS);
}

DWORD __wrap_wGetFileAttributes(wchar_t *path) {
    return mock_type(DWORD);
}

BOOL __wrap_wCreatePipe(PHANDLE hReadPipe,
                        PHANDLE hWritePipe,
                        LPSECURITY_ATTRIBUTES lpPipeAttributes,
                        DWORD nSize) {
    return mock_type(BOOL);
}

wchar_t *__wrap__wgetcwd(wchar_t *buffer, int maxlen) {
    return mock_ptr_type(wchar_t *);
}

LSTATUS
__wrap_wRegOpenKeyExW(
    HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    return mock_type(LSTATUS);
}

BOOL __wrap_wCreateDirectoryW(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
    return mock_type(BOOL);
}

BOOL __wrap_wPathAppendW(LPWSTR pszPath, LPCWSTR pszMore) {
    LPCWSTR newPszPath = mock_ptr_type(LPWSTR);
    wmemcpy(pszPath, newPszPath, wcslen(newPszPath));
    return mock_type(BOOL);
}

int __wrap_wcsicmp(wchar_t *a, wchar_t *b) {
    return mock_type(int);
}

DWORD __wrap_wFormatMessageW(DWORD dwFlags,
                             LPCVOID lpSource,
                             DWORD dwMessageId,
                             DWORD dwLanguageId,
                             LPWSTR lpBuffer,
                             DWORD nSize,
                             va_list *Arguments) {
    lpBuffer[0] = L'\n';
    return mock_type(DWORD);
}

BOOL __wrap__CrtDumpMemoryLeaks() {
    return FALSE;
}

HRESULT
__wrap_wSHGetFolderPathW(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath) {
    LPWSTR newPszPath = mock_ptr_type(LPWSTR);
    wmemcpy(pszPath, newPszPath, wcslen(newPszPath));
    return mock_type(HRESULT);
}
