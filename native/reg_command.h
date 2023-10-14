#ifndef REG_COMMAND_H
#define REG_COMMAND_H

#include "unicode.h"
#include <windows.h>

void do_write_reg_command(HKEY hk,
                          const char *prior_stem,
                          const char *prop,
                          const unsigned char *value,
                          size_t value_len,
                          DWORD type,
                          bool debug);
void do_write_reg_commands(HKEY hk, unsigned n_values, const char *prior_stem, bool debug);

#endif // REG_COMMAND_H
