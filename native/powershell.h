#ifndef POWERSHELL_H
#define POWERSHELL_H

#include "io.h"

bool do_write_powershell_reg_code(writer_t *writer,
                                  const wchar_t *full_path,
                                  const wchar_t *prop,
                                  const char *value,
                                  size_t data_len,
                                  unsigned long type);

#endif // POWERSHELL_H
