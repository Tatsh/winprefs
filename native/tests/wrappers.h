#ifndef TESTS_WRAPPERS_H
#define TESTS_WRAPPERS_H

#if !HAVE_SNWPRINTF
#define _snwprintf swprintf
#endif
#if !HAVE_WCSNICMP
#define _wcsnicmp wcsncmp
#endif

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
typedef long LONG;
typedef LONG HRESULT, LSTATUS;
typedef wchar_t WCHAR;
typedef WCHAR *LPWSTR;
typedef const WCHAR *LPCWSTR;
typedef const WCHAR *LPCWCH, *PCWCH, *LMCSTR, *LPCWSTR, *PCWSTR;
typedef bool BOOL, *LPBOOL;
typedef unsigned int UINT;
typedef void *HANDLE, *PHANDLE, *LPVOID, *LPOVERLAPPED, *HWND;
typedef const void *LPCVOID;
typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;
typedef struct _STARTUPINFOW {
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
} STARTUPINFOW, *LPSTARTUPINFOW;
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

#define BOOL bool
#define CP_UTF8 0
#define CREATE_ALWAYS 2
#define CREATE_NEW_CONSOLE 0
#define CSIDL_APPDATA 0
#define ERROR_ACCESS_DENIED 802
#define ERROR_ALREADY_EXISTS 803
#define ERROR_MORE_DATA 800
#define ERROR_NO_MORE_ITEMS 801
#define ERROR_SUCCESS 0
#define FALSE 0
#define FILE_ATTRIBUTE_DIRECTORY 0
#define FILE_ATTRIBUTE_NORMAL 2
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
#define INVALID_FILE_ATTRIBUTES 1 << 2
#define INVALID_HANDLE_VALUE (void *)0
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
#define TRUE 1
#define WAIT_OBJECT_0 0

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
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
BOOL CreatePipe(PHANDLE hReadPipe,
                PHANDLE hWritePipe,
                LPSECURITY_ATTRIBUTES lpPipeAttributes,
                DWORD nSize);
BOOL CloseHandle(HANDLE hObject);
DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
BOOL PeekNamedPipe(HANDLE hNamedPipe,
                   LPVOID lpBuffer,
                   DWORD nBufferSize,
                   LPDWORD lpBytesRead,
                   LPDWORD lpTotalBytesAvail,
                   LPDWORD lpBytesLeftThisMessage);
BOOL ReadFile(HANDLE hFile,
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
                        LPBOOL lpUsedDefaultChar);
void PathStripPath(LPWSTR pszPath);
BOOL CreateProcess(LPCWSTR lpApplicationName,
                   LPWSTR lpCommandLine,
                   LPSECURITY_ATTRIBUTES lpProcessAttributes,
                   LPSECURITY_ATTRIBUTES lpThreadAttributes,
                   BOOL bInheritHandles,
                   DWORD dwCreationFlags,
                   LPVOID lpEnvironment,
                   LPCWSTR lpCurrentDirectory,
                   LPSTARTUPINFOW lpStartupInfo,
                   LPPROCESS_INFORMATION lpProcessInformation);
BOOL WriteFile(HANDLE hFile,
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
BOOL CreateDirectory(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
BOOL PathAppend(LPWSTR pszPath, LPCWSTR pszMore);
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
intptr_t _wspawnlp(int mode, const wchar_t *cmd, const wchar_t *_ArgList, ...);
wchar_t *_wgetcwd(wchar_t *buffer, int maxlen);
int _wchdir(const wchar_t *dirname);
int _snwprintf(wchar_t *buffer, size_t count, const wchar_t *format, ...);
int wcsicmp(const wchar_t *a, const wchar_t *b);
BOOL _CrtDumpMemoryLeaks();
DWORD FormatMessage(DWORD dwFlags,
                    LPCVOID lpSource,
                    DWORD dwMessageId,
                    DWORD dwLanguageId,
                    LPWSTR lpBuffer,
                    DWORD nSize,
                    va_list *Arguments);

#endif // TESTS_WRAPPERS_H
