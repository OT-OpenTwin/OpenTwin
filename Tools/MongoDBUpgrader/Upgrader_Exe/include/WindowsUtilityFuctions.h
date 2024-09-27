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
    const std::wstring toWideString(const std::string& _string);
	
    class RegistryHandler
    {
    public:
        RegistryHandler(const std::string& _strKey);
		~RegistryHandler();
		bool setStringVal(const std::string& _entryName, const std::string& _entrydata);

		std::string getStringVal(const std::string& _entryName);

    private:
		HKEY m_hKey;
    };

}
