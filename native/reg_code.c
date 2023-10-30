#include "reg_code.h"
#include "registry.h"

bool do_write_c_reg_code(HANDLE out_fp,
                         const wchar_t *full_path,
                         const wchar_t *prop,
                         const char *value,
                         size_t data_len,
                         unsigned long type) {
    (void)full_path;
    (void)prop;
    (void)data_len;
    (void)out_fp;
    (void)type;
    (void)value;
    return false;
}

bool do_write_c_sharp_reg_code(HANDLE out_fp,
                               const wchar_t *full_path,
                               const wchar_t *prop,
                               const char *value,
                               size_t data_len,
                               unsigned long type) {
    (void)full_path;
    (void)prop;
    (void)data_len;
    (void)out_fp;
    (void)type;
    (void)value;
    return false;
}

bool do_write_powershell_reg_code(HANDLE out_fp,
                                  const wchar_t *full_path,
                                  const wchar_t *prop,
                                  const char *value,
                                  size_t data_len,
                                  unsigned long type) {
    (void)full_path;
    (void)prop;
    (void)data_len;
    (void)out_fp;
    (void)type;
    (void)value;
    return false;
}
