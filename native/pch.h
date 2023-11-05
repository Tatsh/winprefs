#ifndef PCH_H
#define PCH_H

#include <errno.h>
#include <setjmp.h>
#if HAVE_STDBOOL
#include <stdbool.h>
#endif
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#if !defined(TESTING) || !TESTING
#include <crtdbg.h>
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>
// Must come after windows.h
#include <versionhelpers.h>
#else
#include <cmocka.h>

#include "tests/wrappers.h"
#endif

#include "debug.h"
#include "macros.h"

#endif // PCH_H
