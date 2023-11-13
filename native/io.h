#ifndef IO_H
#define IO_H

enum OUTPUT_FORMAT {
    OUTPUT_FORMAT_C,
    OUTPUT_FORMAT_C_SHARP,
    OUTPUT_FORMAT_POWERSHELL,
    OUTPUT_FORMAT_REG,
    OUTPUT_FORMAT_UNKNOWN
};

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

static inline size_t determine_multi_sz_size(wchar_t *w_data, size_t hint) {
    if (hint == 0) {
        return 0;
    }
    int i;
    // Validate the string has correct null terminators
    for (i = 0; i < (hint - 1); i++) {
        wchar_t a = w_data[i];
        wchar_t b = w_data[i + 1];
        if (a == L'\0' && b == L'\0') {
            return i + 1;
        }
    }
    return 0;
}

#endif // IO_H
