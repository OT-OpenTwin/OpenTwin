#include "WindowsUtilityFuctions.h"
#include <stdio.h>
#include <malloc.h>

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

const std::wstring wuf::toWideString(const std::string& _string)
{
    int wideCharSize = MultiByteToWideChar(CP_UTF8, 0, _string.c_str(), -1, NULL, 0);
    wchar_t* wideCharStr = new wchar_t[wideCharSize];
    MultiByteToWideChar(CP_UTF8, 0, _string.c_str(), -1, wideCharStr, wideCharSize);
    std::wstring wstring(wideCharStr);
    delete[] wideCharStr;
    return wstring;
}

wuf::RegistryHandler::RegistryHandler(const std::string& _strKey)
{
    LONG nError = RegOpenKeyExA(HKEY_LOCAL_MACHINE, _strKey.c_str(), NULL, KEY_ALL_ACCESS, &m_hKey);
   
    if (nError != ERROR_SUCCESS)
    {
        throw std::exception(("Failed to access registry entry :" + _strKey + " due to Error" + wuf::getErrorMessage(nError)).c_str());
    }
}

wuf::RegistryHandler::~RegistryHandler()
{
    RegCloseKey(m_hKey);
}

bool wuf::RegistryHandler::setStringVal(const std::string& _entryName, const std::string& _entrydata)
{
    LONG setRes = RegSetValueExA(m_hKey, _entryName.c_str(), 0, REG_SZ, (LPBYTE)_entrydata.c_str(), _entrydata.size());
    return setRes == ERROR_SUCCESS;
}

std::string wuf::RegistryHandler::getStringVal(const std::string& _entryName)
{
    DWORD BufferSize = 8192;
    DWORD cbData;
    DWORD dwRet;

    char* data = (char*)malloc(BufferSize);
    cbData = BufferSize;
    std::wstring wStringName = toWideString(_entryName);
    dwRet = RegQueryValueExA(m_hKey,
        _entryName.c_str(),
        NULL,
        NULL,
        (LPBYTE)data,
        &cbData);
    std::string value(data);

    return value;
}
