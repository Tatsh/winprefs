/** \file */
#ifdef ENABLE_VLD
#include <vld.h>
#endif

#include "arg.h"
#include "constants.h"
#include "git.h"
#include "io.h"
#include "reg_code.h"
#include "reg_command.h"
#include "registry.h"

static inline void print_leaks() {
#ifndef ENABLE_VLD
    if (debug_print_enabled) {
        _CrtDumpMemoryLeaks();
    }
#endif
}

//! Gets the `HKEY` pointer for the first part of a registry path.
/*!
 \param reg_path Full registry path.
 \return Pointer to root key handle.
 */
static inline HKEY get_top_key(wchar_t *reg_path) {
    if (!_wcsnicmp(reg_path, L"HKCU", 4) || !_wcsnicmp(reg_path, L"HKEY_CURRENT_USER", 17)) {
        return HKEY_CURRENT_USER;
    }
    if (!_wcsnicmp(reg_path, L"HKCR", 4) || !_wcsnicmp(reg_path, L"HKEY_CLASSES_ROOT", 17)) {
        return HKEY_CLASSES_ROOT;
    }
    if (!_wcsnicmp(reg_path, L"HKLM", 4) || !_wcsnicmp(reg_path, L"HKEY_LOCAL_MACHINE", 18)) {
        return HKEY_LOCAL_MACHINE;
    }
    if (!_wcsnicmp(reg_path, L"HKCC", 4) || !_wcsnicmp(reg_path, L"HKEY_CURRENT_CONFIG", 19)) {
        return HKEY_CURRENT_CONFIG;
    }
    if (!_wcsnicmp(reg_path, L"HKU", 3) || !_wcsnicmp(reg_path, L"HKEY_USERS", 10)) {
        return HKEY_USERS;
    }
    if (!_wcsnicmp(reg_path, L"HKDD", 4) || !_wcsnicmp(reg_path, L"HKEY_DYN_DATA", 13)) {
        return HKEY_DYN_DATA;
    }
    return nullptr;
}

//! Entry point.
int wmain(int argc, wchar_t *argv[]) {
    (void)argc;
    int ret = EXIT_SUCCESS;
    HKEY starting_key = HKEY_CURRENT_USER;
    bool commit = false;
    bool debug = false;
    int max_depth = 20;
    char *as_char = nullptr;
    wchar_t *argv0 = argv[0];
    wchar_t *deploy_key = nullptr;
    wchar_t *format = nullptr;
    wchar_t *output_dir = nullptr;
    wchar_t *output_file = nullptr;
    bool output_dir_specified = false;
    ARG_BEGIN {
        if (ARG_LONG("deploy-key"))
        case 'K': {
            deploy_key = ARG_VAL();
        }
        else if (ARG_LONG("commit")) case 'c': {
            commit = true;
            ARG_FLAG();
        }
        else if (ARG_LONG("debug")) case 'd': {
            debug = true;
            ARG_FLAG();
        }
        else if (ARG_LONG("output-file")) case 'f': {
            output_file = ARG_VAL();
        }
        else if (ARG_LONG("output-dir")) case 'o': {
            output_dir = ARG_VAL();
            output_dir_specified = true;
        }
        else if (ARG_LONG("format")) case 'F': {
            format = ARG_VAL();
        }
        else if (ARG_LONG("max-depth")) case 'm': {
            wchar_t *val = ARG_VAL();
            size_t w_len = wcslen(val);
            as_char = malloc(w_len + 1);
            if (!as_char) { // LCOV_EXCL_START
                fprintf(stderr, "Memory error.\n");
                goto fail;
            } // LCOV_EXCL_STOP
            memset(as_char, 0, w_len + 1);
            wcstombs(as_char, val, w_len);
            max_depth = atoi(as_char);
        }
        else if (ARG_LONG("help")) case 'h':
        case '?': {
            PathStripPath(argv0);
            wprintf(L"Usage: %ls [OPTION...] [REG_PATH]\n", argv0);
            wprintf(L"\nIf a path to a value name is specified, the output directory argument is "
                    L"ignored and the line is printed to\nstandard output.\n\n");
            puts("Options:");
            puts("  -F, --format=FORMAT Format to output. Options: c, cs, c#, ps, ps1, "
                 "powershell, reg. Default: reg.");
            puts("  -K, --deploy-key    Deploy key for committing.");
            puts("  -c, --commit        Commit changes.");
            puts("  -d, --debug         Enable debug logging.");
            puts("  -f, --output-file   Output filename.");
            puts("  -m, --max-depth=INT Set maximum depth.");
            puts("  -o, --output-dir    Output directory.");
            puts("  -h, --help          Display this help and exit.");
            return EXIT_SUCCESS;
        }
            else {
            default:
                fwprintf(stderr,
                         L"%ls: invalid option '%ls'\n"
                         L"Try '%s --help' for more information.\n",
                         argv0,
                         *argv,
                         argv0);
                goto fail;
            }
    }
    ARG_END;
    wchar_t *reg_path = *argv;
    enum OUTPUT_FORMAT output_format_e =
        (!format || !_wcsicmp(L"reg", format)) ? OUTPUT_FORMAT_REG :
        (!_wcsicmp(L"c#", format) || !_wcsicmp(L"cs", format) || !_wcsicmp(L"csharp", format)) ?
                                                 OUTPUT_FORMAT_C_SHARP :
        (!_wcsicmp(L"powershell", format) || !_wcsicmp(L"ps", format) ||
         !_wcsicmp(L"ps1", format)) ?
                                                 OUTPUT_FORMAT_POWERSHELL :
        (format[0] == L'c' || format[0] == L'C') ? OUTPUT_FORMAT_C :
                                                   OUTPUT_FORMAT_UNKNOWN;
    if (output_format_e == OUTPUT_FORMAT_UNKNOWN) {
        fwprintf(stderr, L"Unknown format specified: %ls\n", format);
        goto fail;
    }
    if (reg_path) {
        size_t len = wcslen(reg_path);
        fwprintf(stderr, L"@@ %ls, %lc\n", reg_path, reg_path[len - 1]);
        bool top_key_only =
            (reg_path[len - 1] == L'\\' && reg_path[len - 2] == L':') || reg_path[len - 1] == L':';
        if (reg_path[len - 1] == L'\\' && reg_path[len - 2] == L':') {
            reg_path[len - 2] = L'\0';
        } else if (reg_path[len - 1] == L':') {
            reg_path[len - 1] = L'\0';
        }
        HKEY top_key = get_top_key(reg_path);
        if (!top_key) {
            fwprintf(stderr, L"Invalid top-level key in '%ls'.\n", reg_path);
            goto fail;
        }
        if (!top_key_only) {
            wchar_t *subkey = wcschr(reg_path, L'\\') + 1;
            if (RegOpenKeyEx(top_key, subkey, 0, KEY_READ, &starting_key) != ERROR_SUCCESS) {
                // See if it's a full path to value
                ret = export_single_value(top_key, reg_path, output_format_e) ? EXIT_SUCCESS :
                                                                                EXIT_FAILURE;
                goto cleanup;
            }
        }
    }
    if (!output_dir) {
        output_dir = calloc(MAX_PATH, WL);
        if (!output_dir) { // LCOV_EXCL_START
            fprintf(stderr, "Failed to allocate memory.\n");
            return EXIT_FAILURE;
        } // LCOV_EXCL_STOP
        wmemset(output_dir, L'\0', MAX_PATH);
        if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0, output_dir))) {
            PathAppend(output_dir, L"prefs-export");
        }
        output_dir[MAX_PATH - 1] = L'\0';
    }
    debug_print_enabled = debug;
    bool success = save_preferences(
        commit,
        deploy_key,
        output_dir,
        output_file ? output_file :
                      (output_format_e == OUTPUT_FORMAT_C          ? L"exec-reg.c" :
                       output_format_e == OUTPUT_FORMAT_POWERSHELL ? L"exec-reg.ps1" :
                       output_format_e == OUTPUT_FORMAT_C_SHARP    ? L"exec-reg.cs" :
                                                                     L"exec-reg.bat"),
        max_depth,
        starting_key,
        reg_path,
        output_format_e);
    if (!success) {
        fwprintf(stderr, L"Error occurred. Possibilities:\n");
        DWORD last_win_error = GetLastError();
        wchar_t p_message_buf[8192];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      nullptr,
                      last_win_error,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR)&p_message_buf,
                      8192,
                      nullptr);
        fprintf(stderr, "POSIX   (%d): %s\n", errno, strerror(errno));
        fwprintf(stderr, L"Windows (%d): %ls", last_win_error, p_message_buf);
        goto fail;
    }
    goto cleanup;
fail:
    ret = EXIT_FAILURE;
cleanup:
    free_if_not_null(as_char);
    if (!output_dir_specified) {
        free_if_not_null(output_dir);
    }
    print_leaks();
    return ret;
}
