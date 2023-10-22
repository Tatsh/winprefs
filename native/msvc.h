#ifndef MSVC_H
#define MSVC_H

#ifdef _MSC_VER
#define nullptr NULL
#endif

#ifndef _MSC_VER
#include <stdbool.h>
#else
#define bool BOOL
#define true TRUE
#define false FALSE
#endif

#ifndef WC_ERR_INVALID_CHARS
#define WC_ERR_INVALID_CHARS 0
#define _WC_ERR_INVALID_CHARS 0x0080
#endif

#endif // MSVC_H
