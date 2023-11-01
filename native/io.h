#ifndef IO_H
#define IO_H

#include <windef.h>

#include "macros.h"

bool write_output(HANDLE out_fp, wchar_t *out, bool use_crlf);

#endif // IO_H
