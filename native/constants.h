/** \file */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stddef.h>

//! Maximum `cmd.exe` command line length.
extern const size_t CMD_MAX_COMMAND_LENGTH;
//! Maximum length of a registry key.
extern const size_t MAX_KEY_LENGTH;
//! Maximum length of the value-name in a `reg add` command.
extern const size_t MAX_VALUE_NAME;
//! Alias of `sizeof(wchar_t)`.
extern const size_t WL;
extern const wchar_t *AUTOMATIC_COMMIT_MESSAGE_PREFIX;
extern const wchar_t *C_REGSETKEYVALUEW_TEMPLATE_BINARY;
extern const wchar_t *C_REGSETKEYVALUEW_TEMPLATE_NUMERIC;
extern const wchar_t *C_REGSETKEYVALUEW_TEMPLATE_SZ;
extern const wchar_t *C_SHARP_REGISTRY_SET_VALUE_TEMPLATE;
extern const wchar_t *KEYWORD_DWORD;
extern const wchar_t *KEYWORD_QWORD;
extern const wchar_t *POWERSHELL_NEW_ITEMPROPERTY_TEMPLATE;
extern const wchar_t *POWERSHELL_NEW_ITEM_TEMPLATE;

#endif // CONSTANTS_H
