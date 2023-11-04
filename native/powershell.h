#ifndef POWERSHELL_H
#define POWERSHELL_H

bool do_write_powershell_reg_code(HANDLE out_fp,
                                  const wchar_t *full_path,
                                  const wchar_t *prop,
                                  const char *value,
                                  size_t data_len,
                                  unsigned long type);

#endif // POWERSHELL_H
