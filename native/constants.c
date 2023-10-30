#include "constants.h"

const size_t CMD_MAX_COMMAND_LENGTH = 8191;
const size_t MAX_KEY_LENGTH = 255;
const size_t MAX_VALUE_NAME = 16383;
const size_t WL = sizeof(wchar_t);
const wchar_t *AUTOMATIC_COMMIT_MESSAGE_PREFIX = L"Automatic commit @ ";

const wchar_t *POWERSHELL_NEW_ITEM_TEMPLATE = L"New-Item -Path '%ls' -Force | Out-Null";
const wchar_t *POWERSHELL_NEW_ITEMPROPERTY_TEMPLATE =
    L"New-ItemProperty -Path '%ls' -Name '%ls' -Value '%ls' -PropertyType '%ls' -Force";

const wchar_t *C_SHARP_REGISTRY_SET_VALUE_TEMPLATE =
    L"Registry.SetValue(\"%ls\", \"%ls\", \"%ls\", %ls)";

const wchar_t *C_REGSETKEYVALUEW_TEMPLATE_SZ =
    L"RegSetKeyValueW(%ls, L\"%ls\", L\"%ls\", %ls, L\"%ls\", %ld)";
const wchar_t *C_REGSETKEYVALUEW_TEMPLATE_BINARY =
    L"data = (VOID)%ls; RegSetKeyValueW(%ls, L\"%ls\", L\"%ls\", %ls, data, %ld)";

const wchar_t *KEYWORD_DWORD = L"DWORD";
const wchar_t *KEYWORD_QWORD = L"QWORD";
const wchar_t *C_REGSETKEYVALUEW_TEMPLATE_NUMERIC =
    L"%lsnum = %ld; RegSetKeyValueW(%ls, L\"%ls\", L\"%ls\", %ls, (LPCVOID)&num, sizeof(%ls))";
