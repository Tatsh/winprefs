#ifndef TESTS_WRAPPERS_H
#define TESTS_WRAPPERS_H

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define UINT64 uint64_t
typedef short WORD;
typedef char CHAR;
typedef CHAR *LPSTR, *LPCCH;
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;
typedef char byte, BYTE, *PBYTE, *LPBYTE;
typedef unsigned long DWORD, LCID, *LPDWORD, REGSAM;
typedef unsigned long long QWORD;
typedef long LONG;
typedef LONG HRESULT, LSTATUS;
typedef wchar_t WCHAR;
typedef WCHAR *LPWSTR;
typedef const WCHAR *LPCWSTR;
typedef const WCHAR *LPCWCH, *PCWCH, *LMCSTR, *LPCWSTR, *PCWSTR;
typedef unsigned int UINT;
typedef void *HANDLE, *PHANDLE, *LPVOID, *LPOVERLAPPED, *HWND;
typedef const void *LPCVOID;
typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    bool bInheritHandle;
} SECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;
typedef struct _STARTUPINFO {
    DWORD cb;
    LPWSTR lpReserved;
    LPWSTR lpDesktop;
    LPWSTR lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD wShowWindow;
    WORD cbReserved2;
    LPBYTE lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFO, *LPSTARTUPINFO;
typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;
typedef HANDLE HKEY, *PHKEY;
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME;
typedef struct _FILETIME *PFILETIME;
typedef WCHAR *LPTSTR;
const enum REG_TYPES {
    REG_BINARY,
    REG_DWORD,
    REG_EXPAND_SZ,
    REG_MULTI_SZ,
    REG_NONE,
    REG_QWORD,
    REG_SZ,
};

#define CP_UTF8 0
#define CREATE_ALWAYS 2
#define CREATE_NO_WINDOW 0
#define CSIDL_APPDATA 0
#define ERROR_ACCESS_DENIED 802
#define ERROR_ALREADY_EXISTS 803
#define ERROR_MORE_DATA 800
#define ERROR_NO_MORE_ITEMS 801
#define ERROR_SUCCESS 0

#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FORMAT_MESSAGE_FROM_SYSTEM 2
#define FORMAT_MESSAGE_IGNORE_INSERTS 1
#define GENERIC_READ 0
#define GENERIC_WRITE 1
#define HKEY_CLASSES_ROOT (HKEY)2
#define HKEY_CURRENT_CONFIG (HKEY)4
#define HKEY_CURRENT_USER (HKEY)1
#define HKEY_DYN_DATA (HKEY)6
#define HKEY_LOCAL_MACHINE (HKEY)3
#define HKEY_USERS (HKEY)5
#define INFINITE 0xFFFFFFFF
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#define INVALID_HANDLE_VALUE (void *)3
#define IsWindowsVistaOrGreater() 0
#define KEY_READ 0
#define LOCALE_USER_DEFAULT 0
#define MAKELANGID(x, y) 0
#define MAX_PATH 260
#define P_WAIT 0
#define STARTF_USESHOWWINDOW 1
#define STARTF_USESTDHANDLES 2
#define STD_OUTPUT_HANDLE 0
#define SUCCEEDED(x) x == 0
#define SW_HIDE 0
#define WAIT_OBJECT_0 0

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef TEXT
#define TEXT(quote) L##quote
#endif

DWORD GetFileAttributes(wchar_t *path);
int GetTimeFormat(LCID Locale,
                  DWORD dwFlags,
                  const SYSTEMTIME *lpTime,
                  LPCWSTR lpFormat,
                  LPWSTR lpTimeStr,
                  int cchTime);
int GetDateFormat(LCID Locale,
                  DWORD dwFlags,
                  const SYSTEMTIME *lpDate,
                  LPCWSTR lpFormat,
                  LPWSTR lpDateStr,
                  int cchDate);
bool CreatePipe(PHANDLE hReadPipe,
                PHANDLE hWritePipe,
                LPSECURITY_ATTRIBUTES lpPipeAttributes,
                DWORD nSize);
bool CloseHandle(HANDLE hObject);
DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
bool PeekNamedPipe(HANDLE hNamedPipe,
                   LPVOID lpBuffer,
                   DWORD nBufferSize,
                   LPDWORD lpBytesRead,
                   LPDWORD lpTotalBytesAvail,
                   LPDWORD lpBytesLeftThisMessage);
bool ReadFile(HANDLE hFile,
              LPVOID lpBuffer,
              DWORD nNumberOfBytesToRead,
              LPDWORD lpNumberOfBytesRead,
              LPOVERLAPPED lpOverlapped);
int MultiByteToWideChar(UINT CodePage,
                        DWORD dwFlags,
                        LPCCH lpMultiByteStr,
                        int cbMultiByte,
                        LPWSTR lpWideCharStr,
                        int cchWideChar);
int WideCharToMultiByte(UINT CodePage,
                        DWORD dwFlags,
                        LPCWCH lpWideCharStr,
                        int cchWideChar,
                        LPSTR lpMultiByteStr,
                        int cbMultiByte,
                        LPCCH lpDefaultChar,
                        bool *lpUsedDefaultChar);
void PathStripPath(LPWSTR pszPath);
bool CreateProcess(LPCWSTR lpApplicationName,
                   LPWSTR lpCommandLine,
                   SECURITY_ATTRIBUTES *lpProcessAttributes,
                   SECURITY_ATTRIBUTES *lpThreadAttributes,
                   bool bInheritHandles,
                   DWORD dwCreationFlags,
                   void *lpEnvironment,
                   LPCWSTR lpCurrentDirectory,
                   LPSTARTUPINFO lpStartupInfo,
                   LPPROCESS_INFORMATION lpProcessInformation);
bool WriteFile(HANDLE hFile,
               LPCVOID lpBuffer,
               DWORD nNumberOfBytesToWrite,
               LPDWORD lpNumberOfBytesWritten,
               LPOVERLAPPED lpOverlapped);
LSTATUS RegEnumValue(HKEY hKey,
                     DWORD dwIndex,
                     LPWSTR lpValueName,
                     LPDWORD lpcchValueName,
                     LPDWORD lpReserved,
                     LPDWORD lpType,
                     LPBYTE lpData,
                     LPDWORD lpcbData);
DWORD GetLastError();
bool CreateDirectory(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
bool PathAppend(LPWSTR pszPath, LPCWSTR pszMore);
HANDLE GetStdHandle(DWORD nStdHandle);
LSTATUS
RegOpenKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
LSTATUS RegEnumKeyEx(HKEY hKey,
                     DWORD dwIndex,
                     LPWSTR lpName,
                     LPDWORD lpcchName,
                     LPDWORD lpReserved,
                     LPWSTR lpClass,
                     LPDWORD lpcchClass,
                     PFILETIME lpftLastWriteTime);
LSTATUS RegCloseKey(HKEY hKey);
HANDLE CreateFile(LPCWSTR lpFileName,
                  DWORD dwDesiredAccess,
                  DWORD dwShareMode,
                  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                  DWORD dwCreationDisposition,
                  DWORD dwFlagsAndAttributes,
                  HANDLE hTemplateFile);
LSTATUS RegQueryInfoKey(HKEY hKey,
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
                        PFILETIME lpftLastWriteTime);
LSTATUS RegQueryValueEx(HKEY hKey,
                        LPCWSTR lpValueName,
                        LPDWORD lpReserved,
                        LPDWORD lpType,
                        LPBYTE lpData,
                        LPDWORD lpcbData);
HRESULT SHGetFolderPath(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath);
wchar_t *_wfullpath(wchar_t *absPath, const wchar_t *relPath, size_t maxLength);
wchar_t *_wgetcwd(wchar_t *buffer, int maxlen);
int _wchdir(const wchar_t *dirname);
int _snwprintf(wchar_t *buffer, size_t count, const wchar_t *format, ...);
int _wcsicmp(const wchar_t *a, const wchar_t *b);
bool _CrtDumpMemoryLeaks();
DWORD FormatMessage(DWORD dwFlags,
                    LPCVOID lpSource,
                    DWORD dwMessageId,
                    DWORD dwLanguageId,
                    LPWSTR lpBuffer,
                    DWORD nSize,
                    va_list *Arguments);
bool GetExitCodeProcess(HANDLE hProcess, LPDWORD lpExitCode);

#endif // TESTS_WRAPPERS_H
