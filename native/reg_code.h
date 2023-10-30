#ifndef REG_CODE_H
#define REG_CODE_H

#include <windows.h>

#include "macros.h"

typedef bool (*do_write_callback)(
    HANDLE, const wchar_t *, const wchar_t *, const char *, size_t, unsigned long);

bool do_write_c_reg_code(HANDLE out_fp,
                         const wchar_t *full_path,
                         const wchar_t *prop,
                         const char *value,
                         size_t data_len,
                         unsigned long type);
bool do_write_c_sharp_reg_code(HANDLE out_fp,
                               const wchar_t *full_path,
                               const wchar_t *prop,
                               const char *value,
                               size_t data_len,
                               unsigned long type);
bool do_write_powershell_reg_code(HANDLE out_fp,
                                  const wchar_t *full_path,
                                  const wchar_t *prop,
                                  const char *value,
                                  size_t data_len,
                                  unsigned long type);

#endif // REG_CODE_H
