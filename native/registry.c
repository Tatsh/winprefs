#include "registry.h"
#include "constants.h"
#include "git.h"
#include "io.h"
#include "io_default_writer.h"
#include "reg_code.h"
#include "reg_command.h"

static inline bool create_dir_wrapper(wchar_t *folder) {
    if (!CreateDirectory(folder, nullptr)) {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS && err != ERROR_ACCESS_DENIED) {
            return false;
        }
    }
    return true;
}

static inline bool create_dir_recursive(wchar_t *path) {
    wchar_t folder[MAX_PATH];
    wchar_t *end;
    wmemset(folder, L'\0', MAX_PATH);
    end = wcschr(path, L'\\');
    while (end != nullptr) {
        wcsncpy(folder, path, (size_t)(end - path + 1));
        if (!create_dir_wrapper(folder)) {
            return false;
        }
        end = wcschr(++end, L'\\');
    }
    if (!create_dir_wrapper(path)) {
        return false;
    }
    return true;
}

static void install_default_filters(HKEY top_key) {
    HKEY filter_hkey = nullptr;
    LSTATUS status =
        RegOpenKeyEx(top_key, L"Software\\Tatsh\\WinPrefs\\Filters", 0, KEY_READ, &filter_hkey);
    if (status != ERROR_SUCCESS) {
        HKEY base_hkey = nullptr;
        status = RegCreateKeyEx(top_key,
                                L"Software\\Tatsh\\WinPrefs\\Filters",
                                0,
                                nullptr,
                                0,
                                KEY_WRITE,
                                nullptr,
                                &base_hkey,
                                nullptr);
        if (status == ERROR_SUCCESS) {
            const wchar_t *default_filters[] = {
                L"*\\*MRU",
                L"*\\Cache",
                L"*\\Classes\\Extensions\\ContractId\\Windows.BackgroundTasks\\PackageId",
                L"*\\CurrentVersion\\Authentication\\LogonUI\\Notifications\\BackgroundCapability",
                L"*\\CurrentVersion\\CloudStore\\Store\\DefaultAccount\\Current\\*",
                L"*\\Microsoft\\Windows\\Shell\\Bags",
                L"*\\Windows\\Shell\\MuiCache",
            };
            for (size_t i = 0; i < sizeof(default_filters) / sizeof(default_filters[0]); i++) {
                RegSetValueEx(base_hkey,
                              (LPCWSTR)(i == 0 ? L"0" :
                                        i == 1 ? L"1" :
                                        i == 2 ? L"2" :
                                        i == 3 ? L"3" :
                                        i == 4 ? L"4" :
                                        i == 5 ? L"5" :
                                                 L"6"),
                              0,
                              REG_SZ,
                              (const BYTE *)default_filters[i],
                              (DWORD)((wcslen(default_filters[i]) + 1) * WL));
            }
            RegCloseKey(base_hkey);
        }
    }
    RegCloseKey(filter_hkey);
}

static wchar_t *get_filter(size_t *buf_total_wide_chars, size_t *buf_member_size, HKEY top_key) {
    install_default_filters(top_key);
    wchar_t *tmp_buf = nullptr;
    wchar_t *wide_buf = nullptr;
    wchar_t *value_name = nullptr;
    HKEY filter_hkey = nullptr;
    LSTATUS status =
        RegOpenKeyEx(top_key, L"Software\\Tatsh\\WinPrefs\\Filters", 0, KEY_READ, &filter_hkey);
    if (status == ERROR_SUCCESS) {
        DWORD value_count = 0, max_n_wide_chars = 0;
        DWORD max_value_name_len = 0;
        DWORD value_type = REG_NONE;
        status = RegQueryInfoKey(filter_hkey,
                                 nullptr,
                                 nullptr,
                                 nullptr,
                                 nullptr,
                                 nullptr,
                                 nullptr,
                                 &value_count,        // lpcValues
                                 &max_value_name_len, // lpcbMaxValueNameLen
                                 &max_n_wide_chars,   // lpcbMaxValueLen
                                 nullptr,
                                 nullptr);
        if (status == ERROR_SUCCESS && value_count > 0) {
            max_n_wide_chars++;
            DWORD i;
            size_t total_wide_chars = value_count * max_n_wide_chars;
            *buf_member_size = (size_t)max_n_wide_chars;
            *buf_total_wide_chars = total_wide_chars;
            wide_buf = calloc(*buf_total_wide_chars, WL);
            wmemset(wide_buf, L'\0', total_wide_chars);
            tmp_buf = calloc(max_n_wide_chars, WL);
            wmemset(tmp_buf, L'\0', max_n_wide_chars);
            wchar_t *wp = nullptr;
            value_name = calloc(max_value_name_len, WL);
            for (wp = wide_buf, i = 0; i < value_count; i++, wp += max_n_wide_chars) {
                DWORD len = max_n_wide_chars * sizeof(wchar_t); // RegEnumValue expects bytes
                DWORD name_len = max_value_name_len * sizeof(wchar_t);
                status = RegEnumValue(filter_hkey,
                                      i,
                                      value_name,
                                      &name_len,
                                      nullptr,
                                      &value_type,
                                      (LPBYTE)tmp_buf,
                                      &len);
                if ((status != ERROR_SUCCESS && status != ERROR_NO_MORE_ITEMS) ||
                    value_type != REG_SZ) {
                    debug_print(L"Skipping invalid filter value (name = \"%ls\", status = %ld).\n",
                                value_name,
                                status);
                    wmemset(tmp_buf, 0, max_n_wide_chars);
                    continue;
                }
                // len is in bytes. Convert to wide chars.
                size_t len_wide = len / sizeof(wchar_t);
                if (len_wide > 0 && tmp_buf[len_wide - 1] == L'\0') {
                    len_wide--; // don't copy the null terminator
                }
                debug_print(L"Read filter value: `%ls`, len = %zu\n", tmp_buf, len_wide);
                wmemcpy(wp, tmp_buf, len_wide);
                wmemset(tmp_buf, 0, max_n_wide_chars);
            }
            free(value_name);
        }
        free(tmp_buf);
        RegCloseKey(filter_hkey);
    }
    return wide_buf;
}

static bool is_user_admin() {
    BOOL is_admin = false;
    PSID admin_group = nullptr;
    SID_IDENTIFIER_AUTHORITY nt_authority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&nt_authority,
                                 2,
                                 SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS,
                                 0,
                                 0,
                                 0,
                                 0,
                                 0,
                                 0,
                                 &admin_group)) {
        if (!CheckTokenMembership(nullptr, admin_group, &is_admin)) {
            is_admin = false;
        }
        FreeSid(admin_group);
    }
    return is_admin;
}

DLL_EXPORT bool save_preferences(bool commit,
                                 bool read_settings,
                                 const wchar_t *deploy_key,
                                 const wchar_t *output_dir,
                                 const wchar_t *output_file,
                                 int max_depth,
                                 HKEY hk,
                                 const wchar_t *specified_path,
                                 enum OUTPUT_FORMAT format,
                                 writer_t *writer) {
    size_t filter_buf_size = 0;
    size_t filter_member_size = 0;
    wchar_t *filter = read_settings ?
                          get_filter(&filter_buf_size,
                                     &filter_member_size,
                                     is_user_admin() ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER) :
                          nullptr;
    bool using_default_writer, writer_was_setup, writer_was_torn_down;
    using_default_writer = writer_was_setup = writer_was_torn_down = false;
    if (!writer || !writer->write_output) {
        debug_print(L"Using default writer.");
        writer = get_default_writer();
        using_default_writer = true;
    }
    bool ret = true;
    wchar_t full_output_dir[MAX_PATH];
    bool writing_to_stdout = !wcscmp(L"-", output_file);
    if (!_wfullpath(full_output_dir, output_dir, MAX_PATH)) {
        goto fail;
    }
    if (!writing_to_stdout) {
        debug_print(L"Output directory: %ls\n", full_output_dir);
    } else {
        debug_print(L"Writing to standard output.\n");
    }
    if (!writing_to_stdout && !create_dir_recursive(full_output_dir)) {
        goto fail;
    }
    PathAppend(full_output_dir, output_file);
    full_output_dir[MAX_PATH - 1] = L'\0';
    if (writer->setup && !writer->setup(writer, writing_to_stdout, full_output_dir)) {
        goto fail;
    }
    writer_was_setup = true;
    const wchar_t *prior_stem = hk == HKEY_CLASSES_ROOT   ? L"HKCR" :
                                hk == HKEY_CURRENT_CONFIG ? L"HKCC" :
                                hk == HKEY_CURRENT_USER   ? L"HKCU" :
                                hk == HKEY_LOCAL_MACHINE  ? L"HKLM" :
                                hk == HKEY_USERS          ? L"HKU" :
                                hk == HKEY_DYN_DATA       ? L"HKDD" :
                                                            specified_path;
    if (format == OUTPUT_FORMAT_C) {
        DWORD written;
        writer->write_output(writer, C_PREAMBLE, (DWORD)SIZEOF_C_PREAMBLE, &written);
    }
    filter_t filter_info = {
        .buf = filter, .buf_size = filter_buf_size, .member_size = filter_member_size};
    ret = write_key_filtered_recursive(
        hk, nullptr, max_depth, 0, prior_stem, format, writer, &filter_info);
    if (writer_was_setup && writer->teardown) {
        writer->teardown(writer);
        writer_was_torn_down = true;
    }
    if (ret && commit && !writing_to_stdout) {
        git_commit(output_dir, deploy_key);
    }
    goto cleanup;
fail:
    ret = false;
cleanup:
    if (!writer_was_torn_down && writer_was_setup && writer->teardown) {
        // LCOV_EXCL_START
        writer->teardown(writer);
        // LCOV_EXCL_STOP
    }
    if (using_default_writer) {
        free(writer);
    }
    free(filter);
    return ret;
}

DLL_EXPORT bool export_single_value(HKEY top_key,
                                    const wchar_t *reg_path,
                                    enum OUTPUT_FORMAT format,
                                    writer_t *writer) {
    bool using_default_writer, writer_was_setup;
    using_default_writer = writer_was_setup = false;
    if (!writer || !writer->write_output) {
        debug_print(L"Using default writer.");
        writer = get_default_writer();
        using_default_writer = true;
    }
    bool ret = true;
    wchar_t *m_reg_path = nullptr;
    wchar_t *value_name = nullptr;
    char *data = nullptr;
    if (!reg_path) {
        errno = EINVAL;
        goto fail;
    }
    size_t reg_path_len = wcslen(reg_path) + 1;
    m_reg_path = calloc(reg_path_len, WL);
    if (!m_reg_path) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    wmemcpy(m_reg_path, reg_path, reg_path_len);
    wchar_t *first_backslash = wcschr(m_reg_path, L'\\');
    if (!first_backslash) {
        errno = EINVAL;
        goto fail;
    }
    wchar_t *subkey = first_backslash + 1;
    HKEY starting_key = HKEY_CURRENT_USER;
    wchar_t *last_backslash = wcsrchr(m_reg_path, '\\');
    wchar_t *value_name_p = last_backslash + 1;
    size_t value_name_len = wcslen(value_name_p) + 1;
    value_name = calloc(value_name_len, WL);
    if (!value_name) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    wmemset(value_name, L'\0', value_name_len);
    wmemcpy(value_name, value_name_p, value_name_len);
    *last_backslash = L'\0';
    if (RegOpenKeyEx(top_key, subkey, 0, KEY_READ, &starting_key) != ERROR_SUCCESS) {
        debug_print(L"Invalid subkey: '%ls'.\n", subkey);
        goto fail;
    }
    size_t buf_size = 8192;
    data = malloc(buf_size);
    if (!data) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    DWORD reg_type = REG_NONE;
    LSTATUS reg_query_value_ret = RegQueryValueEx(
        starting_key, value_name, nullptr, &reg_type, (LPBYTE)data, (LPDWORD)&buf_size);
    if (reg_query_value_ret == ERROR_MORE_DATA) {
        debug_print(L"Value too large (%ls\\%ls).\n", subkey, value_name);
        goto fail;
    }
    if (reg_query_value_ret != ERROR_SUCCESS) {
        debug_print(L"Invalid value name '%ls'.\n", value_name);
        goto fail;
    }
    if (writer->setup && !writer->setup(writer, true, nullptr)) {
        // LCOV_EXCL_START
        goto fail;
        // LCOV_EXCL_STOP
    }
    writer_was_setup = true;
    switch (format) {
    case OUTPUT_FORMAT_REG:
        if (!do_write_reg_command(writer, reg_path, value_name, data, buf_size, reg_type)) {
            goto fail;
        }
        break;
    case OUTPUT_FORMAT_C:
        if (!do_write_c_reg_code(writer, reg_path, value_name, data, buf_size, reg_type)) {
            goto fail;
        }
        break;
    case OUTPUT_FORMAT_C_SHARP:
        if (!do_write_c_sharp_reg_code(writer, reg_path, value_name, data, buf_size, reg_type)) {
            goto fail;
        }
        break;
    case OUTPUT_FORMAT_POWERSHELL:
        if (!do_write_powershell_reg_code(writer, reg_path, value_name, data, buf_size, reg_type)) {
            goto fail;
        }
        break;
    default:
        errno = EINVAL;
        goto fail;
    }
    goto cleanup;
fail:
    ret = false;
cleanup:
    if (writer_was_setup && writer->teardown) {
        writer->teardown(writer);
    }
    if (using_default_writer) {
        free(writer);
    }
    free(m_reg_path);
    free(value_name);
    free(data);
    return ret;
}
