#include "WindowsUtilityFuctions.h"


std::string wuf::getErrorMessage(DWORD errorCode)
{
    LPVOID lpMsgBuf;
    DWORD bufLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    if (bufLen)
    {
        std::wstring temp((LPTSTR)lpMsgBuf, bufLen);
        LocalFree(lpMsgBuf);
        return wuf::toString(temp);;
    }
    else
    {
        throw std::exception(("failed to interprete system error code: " + std::to_string(errorCode)).c_str());
    }
}

std::string wuf::toString(std::wstring& _wString)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &_wString[0], (int)_wString.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &_wString[0], (int)_wString.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

std::string wuf::getExecutablePath()
{
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);

    //Separate directory path from executable name
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    std::wstring path = (std::wstring(buffer).substr(0, pos));

    return toString(path);
}
