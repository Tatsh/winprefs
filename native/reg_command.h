/** \file */
#ifndef REG_COMMAND_H
#define REG_COMMAND_H

#include "io.h"
#include "reg_code.h"
#include "registry.h"

bool do_write_reg_command(writer_t *writer,
                          const wchar_t *full_path,
                          const wchar_t *prop,
                          const char *value,
                          size_t value_len,
                          unsigned long type);

#endif // REG_COMMAND_H
