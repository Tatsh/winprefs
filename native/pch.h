#ifndef WIN_H
#define WIN_H

#include <errno.h>
#include <setjmp.h>
#if HAVE_STDBOOL
#include <stdbool.h>
#endif
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
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

#include "wrappers.h"
#else
#include "tests/wrappers.h"
#endif

#include "debug.h"
#include "macros.h"

#endif // WIN_H
