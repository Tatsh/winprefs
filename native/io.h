#ifndef IO_H
#define IO_H

enum OUTPUT_FORMAT {
    OUTPUT_FORMAT_C,
    OUTPUT_FORMAT_C_SHARP,
    OUTPUT_FORMAT_POWERSHELL,
    OUTPUT_FORMAT_REG,
    OUTPUT_FORMAT_UNKNOWN
};
typedef bool (*do_write_callback)(
    HANDLE, const wchar_t *, const wchar_t *, const char *, size_t, unsigned long);

bool write_output(HANDLE out_fp, wchar_t *out, bool use_crlf);
bool do_writes(HANDLE out_fp,
               HKEY hk,
               long unsigned int n_values,
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

#endif // IO_H
