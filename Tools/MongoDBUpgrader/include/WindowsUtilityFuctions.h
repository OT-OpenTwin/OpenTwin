#pragma once
#define NOMINMAX //Supress windows macros min max
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

namespace wuf
{
    std::string getErrorMessage(DWORD errorCode);
    std::string toString(std::wstring& _wString);
    std::string getExecutablePath();
}
