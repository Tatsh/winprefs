/** \file */
#ifndef REG_COMMAND_H
#define REG_COMMAND_H

#include <windef.h>

#include "macros.h"
#include "reg_code.h"
#include "registry.h"

bool do_write_reg_command(HANDLE out_fp,
                          const wchar_t *full_path,
                          const wchar_t *prop,
                          const char *value,
                          size_t value_len,
                          unsigned long type);
bool do_writes(HANDLE out_fp,
               HKEY hk,
               unsigned n_values,
               const wchar_t *prior_stem,
               enum OUTPUT_FORMAT format);
//! Starts the registry exporting process.
/*!
 \param hk Key handle.
 \param stem Subkey.
 \param max_depth Maximum key depth.
 \param depth Current depth. Used internally. Should be `0` at start.
 \param out_fp File handle.
 \param prior_stem Last subkey. Used internally. Should be `NULL`.
 \param format Output format.
 \return `true` if successful, `false` otherwise.
 */
bool write_key_filtered_recursive(HKEY hk,
                                  const wchar_t *stem,
                                  int max_depth,
                                  int depth,
                                  HANDLE out_fp,
                                  const wchar_t *prior_stem,
                                  enum OUTPUT_FORMAT format);

#endif // REG_COMMAND_H
