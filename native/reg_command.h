#ifndef REG_COMMAND_H
#define REG_COMMAND_H

#include "unicode.h"

#include <windows.h>

void do_write_reg_command(const wchar_t *full_path,
                          const wchar_t *prop,
                          const char *value,
                          size_t value_len,
                          DWORD type,
                          bool debug);
void do_write_reg_commands(HKEY hk, unsigned n_values, const wchar_t *prior_stem, bool debug);

#endif // REG_COMMAND_H
