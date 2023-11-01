#include <windows.h>
// Must come after
#include <versionhelpers.h>

#include "io.h"
#include "macros.h"

bool write_output(HANDLE out_fp, wchar_t *out, bool use_crlf) {
    size_t addend = use_crlf ? 1 : 0;
    size_t req_size =
        (size_t)WideCharToMultiByte(CP_UTF8,
                                    IsWindowsVistaOrGreater() ? _WC_ERR_INVALID_CHARS : 0,
                                    out,
                                    -1,
                                    nullptr,
                                    0,
                                    nullptr,
                                    nullptr);
    if (req_size == 0) {
        return false;
    }
    size_t total_size = req_size + addend;
    char *mb_out = malloc(total_size);
    if (!mb_out) {
        return false;
    }
    memset(mb_out, 0, total_size);
    WideCharToMultiByte(CP_UTF8,
                        IsWindowsVistaOrGreater() ? _WC_ERR_INVALID_CHARS : 0,
                        out,
                        -1,
                        mb_out,
                        (int)req_size,
                        nullptr,
                        nullptr);
    if (use_crlf) {
        mb_out[total_size - 2] = '\r';
    }
    mb_out[total_size - 1] = '\n';
    DWORD written = 0;
    bool ret = WriteFile(out_fp, mb_out, (DWORD)total_size, &written, nullptr);
    free(mb_out);
    return ret && written > 0;
}
