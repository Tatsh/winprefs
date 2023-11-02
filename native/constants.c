#include "constants.h"

const size_t CMD_MAX_COMMAND_LENGTH = 8191;
const size_t POWERSHELL_MAX_COMMAND_LENGTH = 32764;
const size_t MAX_KEY_LENGTH = 255;
const size_t MAX_VALUE_NAME = 16383;
const size_t WL = sizeof(wchar_t);
const wchar_t *AUTOMATIC_COMMIT_MESSAGE_PREFIX = L"Automatic commit @ ";

const wchar_t *POWERSHELL_CODE_TEMPLATE =
    L"if (!(Test-Path '%ls')) { New-Item -Path '%ls' -Force | Out-Null } New-ItemProperty "
    L"-LiteralPath '%ls' -Name '%ls' -PropertyType %ls -Force -Value %ls";

const wchar_t *C_SHARP_REGISTRY_SET_VALUE_TEMPLATE =
    L"Registry.SetValue(\"%ls\", \"%ls\", \"%ls\", %ls)";

const size_t SIZEOF_C_PREAMBLE = 44;
const char *C_PREAMBLE = "DWORD dnum;\nQWORD qnum;\nunsigned char data;\n";
const wchar_t *C_REGSETKEYVALUEW_TEMPLATE_SZ =
    L"RegSetKeyValueW(%ls, L\"%ls\", L\"%ls\", %ls, L\"%ls\", %ld);";
const wchar_t *C_REGSETKEYVALUEW_TEMPLATE_BINARY =
    L"data = %ls; RegSetKeyValueW(%ls, L\"%ls\", L\"%ls\", REG_BINARY, (LPCVOID)&data, %ld);";
const wchar_t *KEYWORD_DWORD = L"DWORD";
const wchar_t *KEYWORD_QWORD = L"QWORD";
const wchar_t *C_REGSETKEYVALUEW_TEMPLATE_NUMERIC =
    L"%lsnum = %ls; RegSetKeyValueW(%ls, L\"%ls\", L\"%ls\", %ls, (LPCVOID)&num, sizeof(%ls));";
const wchar_t *C_REGSETKEYVALUEW_TEMPLATE_NONE =
    L"RegSetKeyValueW(%ls, L\"%ls\", L\"%ls\", REG_NONE, NULL, 0);";
