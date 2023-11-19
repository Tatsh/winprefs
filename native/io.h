#ifndef IO_H
#define IO_H

#include "io_writer.h"

enum OUTPUT_FORMAT {
    OUTPUT_FORMAT_C,
    OUTPUT_FORMAT_C_SHARP,
    OUTPUT_FORMAT_POWERSHELL,
    OUTPUT_FORMAT_REG,
    OUTPUT_FORMAT_UNKNOWN
};

bool write_output(wchar_t *out, bool use_crlf, writer_t *writer);
bool do_writes(HKEY hk,
               long unsigned int n_values,
               const wchar_t *prior_stem,
               enum OUTPUT_FORMAT format,
               writer_t *writer);
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
                                  const wchar_t *prior_stem,
                                  enum OUTPUT_FORMAT format,
                                  writer_t *writer);

#endif // IO_H
