#include "wrappers.h"
#include "io.h"

bool __wrap_save_preferences(bool commit,
                             const wchar_t *deploy_key,
                             const wchar_t *output_dir,
                             const wchar_t *output_file,
                             int max_depth,
                             HKEY hk,
                             const wchar_t *specified_path,
                             enum OUTPUT_FORMAT format) {
    check_expected(commit);
    check_expected(format);
    check_expected(max_depth);
    check_expected_ptr(deploy_key);
    check_expected_ptr(hk);
    check_expected_ptr(output_dir);
    check_expected_ptr(output_file);
    check_expected_ptr(specified_path);
    return mock_type(bool);
}

bool __wrap_export_single_value(const wchar_t *reg_path, HKEY top_key, enum OUTPUT_FORMAT format) {
    check_expected(format);
    check_expected_ptr(reg_path);
    check_expected_ptr(top_key);
    return mock_type(bool);
}

LSTATUS __wrap_RegCloseKey(HKEY hKey) {
    return mock_type(LSTATUS);
}

intptr_t __wrap__wspawnlp(int mode, const wchar_t *cmd, const wchar_t *arg_list, ...) {
    return mock_type(intptr_t);
}

int __wrap_GetTimeFormat(LCID Locale,
                         DWORD dwFlags,
                         const SYSTEMTIME *lpTime,
                         LPCWSTR lpFormat,
                         LPWSTR lpTimeStr,
                         int cchTime) {
    return mock_type(int);
}

int __wrap_GetDateFormat(LCID Locale,
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

DWORD __wrap_GetLastError() {
    return mock_type(DWORD);
}

int __wrap__wchdir(const wchar_t *dirname) {
    return mock_type(int);
}

BOOL __wrap_CloseHandle(HANDLE hObject) {
    return mock_type(BOOL);
}

BOOL __wrap_CreateProcess(LPCWSTR lpApplicationName,
                          LPWSTR lpCommandLine,
                          LPSECURITY_ATTRIBUTES lpProcessAttributes,
                          LPSECURITY_ATTRIBUTES lpThreadAttributes,
                          BOOL bInheritHandles,
                          DWORD dwCreationFlags,
                          LPVOID lpEnvironment,
                          LPCWSTR lpCurrentDirectory,
                          LPSTARTUPINFO lpStartupInfo,
                          LPPROCESS_INFORMATION lpProcessInformation) {
    return mock_type(BOOL);
}

BOOL __wrap_WriteFile(HANDLE hFile,
                      LPCVOID lpBuffer,
                      DWORD nNumberOfBytesToWrite,
                      LPDWORD lpNumberOfBytesWritten,
                      LPOVERLAPPED lpOverlapped) {
    return mock_type(BOOL);
}

HANDLE __wrap_GetStdHandle(DWORD nStdHandle) {
    return mock_ptr_type(HANDLE);
}

int __wrap_MultiByteToWideChar(UINT CodePage,
                               DWORD dwFlags,
                               LPCCH lpMultiByteStr,
                               int cbMultiByte,
                               LPWSTR lpWideCharStr,
                               int cchWideChar) {
    return mock_type(int);
}

int __wrap_WideCharToMultiByte(UINT CodePage,
                               DWORD dwFlags,
                               LPCWCH lpWideCharStr,
                               int cchWideChar,
                               LPSTR lpMultiByteStr,
                               int cbMultiByte,
                               LPCCH lpDefaultChar,
                               LPBOOL lpUsedDefaultChar) {
    return mock_type(int);
}

void __wrap_PathStripPath(LPWSTR pszPath) {
}

LSTATUS __wrap_RegEnumValue(HKEY hKey,
                            DWORD dwIndex,
                            LPWSTR lpValueName,
                            LPDWORD lpcchValueName,
                            LPDWORD lpReserved,
                            LPDWORD lpType,
                            LPBYTE lpData,
                            LPDWORD lpcbData) {
    return mock_type(LSTATUS);
}

DWORD __wrap_WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds) {
    return mock_type(DWORD);
}

BOOL __wrap_PeekNamedPipe(HANDLE hNamedPipe,
                          LPVOID lpBuffer,
                          DWORD nBufferSize,
                          LPDWORD lpBytesRead,
                          LPDWORD lpTotalBytesAvail,
                          LPDWORD lpBytesLeftThisMessage) {
    return mock_type(BOOL);
}

BOOL __wrap_ReadFile(HANDLE hFile,
                     LPVOID lpBuffer,
                     DWORD nNumberOfBytesToRead,
                     LPDWORD lpNumberOfBytesRead,
                     LPOVERLAPPED lpOverlapped) {
    return mock_type(BOOL);
}

HANDLE __wrap_CreateFile(LPCWSTR lpFileName,
                         DWORD dwDesiredAccess,
                         DWORD dwShareMode,
                         LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                         DWORD dwCreationDisposition,
                         DWORD dwFlagsAndAttributes,
                         HANDLE hTemplateFile) {
    return mock_ptr_type(HANDLE);
}

LSTATUS __wrap_RegEnumKeyEx(HKEY hKey,
                            DWORD dwIndex,
                            LPWSTR lpName,
                            LPDWORD lpcchName,
                            LPDWORD lpReserved,
                            LPWSTR lpClass,
                            LPDWORD lpcchClass,
                            PFILETIME lpftLastWriteTime) {
    return mock_type(LSTATUS);
}

LSTATUS __wrap_RegQueryInfoKey(HKEY hKey,
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

LSTATUS __wrap_RegQueryValueEx(HKEY hKey,
                               LPCWSTR lpValueName,
                               LPDWORD lpReserved,
                               LPDWORD lpType,
                               LPBYTE lpData,
                               LPDWORD lpcbData) {
    return mock_type(LSTATUS);
}

DWORD __wrap_GetFileAttributes(wchar_t *path) {
    return mock_type(DWORD);
}

BOOL __wrap_CreatePipe(PHANDLE hReadPipe,
                       PHANDLE hWritePipe,
                       LPSECURITY_ATTRIBUTES lpPipeAttributes,
                       DWORD nSize) {
    return mock_type(BOOL);
}

wchar_t *__wrap__wgetcwd(wchar_t *buffer, int maxlen) {
    return mock_ptr_type(wchar_t *);
}

LSTATUS
__wrap_RegOpenKeyEx(
    HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    return mock_type(LSTATUS);
}

BOOL __wrap_CreateDirectory(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
    return mock_type(BOOL);
}

BOOL __wrap_PathAppend(LPWSTR pszPath, LPCWSTR pszMore) {
    LPCWSTR newPszPath = mock_ptr_type(LPWSTR);
    if (newPszPath) {
        wmemcpy(pszPath, newPszPath, wcslen(newPszPath));
    }
    return mock_type(BOOL);
}

DWORD __wrap_FormatMessage(DWORD dwFlags,
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
__wrap_SHGetFolderPath(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath) {
    LPWSTR newPszPath = mock_ptr_type(LPWSTR);
    wmemcpy(pszPath, newPszPath, wcslen(newPszPath));
    return mock_type(HRESULT);
}
