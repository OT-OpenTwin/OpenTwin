#include "Varifier.h"
#include <stdlib.h>
#include <string>
#include "Logger.h"
#include "WindowsUtilityFuctions.h"

void Varifier::ensureCorrectMongoEnvVar()
{
	Logger::INSTANCE().write("Search for MongoDB address env var: " + m_MongoAdressEnvVar + "\n");
	char* envVar;
	size_t envVarSize;
	errno_t err = _dupenv_s(&envVar, &envVarSize, m_MongoAdressEnvVar.c_str());
	if (err != NULL)
	{
		throw std::exception(("Failed to get Mongo env var " + m_MongoAdressEnvVar).c_str());
	}
	if (envVar != nullptr)
	{
		std::string mongoPath(envVar);
		Logger::INSTANCE().write("Env var found:"+ mongoPath + "\n");
		if (mongoPath.find("tls@") != std::string::npos)
		{
			mongoPath = mongoPath.substr(4);
			HKEY hKey;
			DWORD dwIndex = 0;
			CHAR lpName[256];
			DWORD cchName = sizeof(lpName) / sizeof(lpName[0]);

			if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\Session Manager\\Environment\\OPEN_TWIN_MONGODB_ADDRESS", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
				//while (RegEnumKeyA(hKey, dwIndex, lpName, cchName) == ERROR_SUCCESS) {
				//	printf("Subkey: %s\n", lpName);
				//	dwIndex++;
				//}
				//RegCloseKey(hKey);
				LONG result = RegSetValueExA(
				hKey, // Root key
				"OPEN_TWIN_MONGODB_ADDRESS",                  // lpValueName
				0,
				REG_SZ,
				(const BYTE*)mongoPath.c_str(),
				mongoPath.size()

			);
			RegCloseKey(hKey);

			}

			//HKEY  keyHandle;
			//LSTATUS status = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
			//	"SYSTEM\\CurrentControlSet\\Control\Session Manager\\Environment",
			//	0,
			//		KEY_SET_VALUE,
			//		&keyHandle
			//	);
			//
			//DWORD index = 0;
			//CHAR valueName[256];
			//BYTE data[256];
			//DWORD valueNameSize, dataSize, type;
			//LPSTR subKeyNam;
			//while (true) {
			//	valueNameSize = sizeof(valueName);
			//	dataSize = sizeof(data);
			//	status = RegEnumValueA(keyHandle, index, valueName, &valueNameSize, NULL, &type, data, &dataSize);
			//	status = RegEnumKeyA(keyHandle, index, valueName, 256);
			//	if (status == ERROR_NO_MORE_ITEMS) {
			//		break;
			//	}
			//	else if (status != ERROR_SUCCESS) {
			//		 Handle error
			//	}

			//	 Process the value (valueName and data)
			//	index++;
			//}


			//LONG result = RegSetValueExA(
			//	keyHandle, // Root key
			//	"OPEN_TWIN_MONGODB_ADDRESS",                  // lpValueName
			//	0,
			//	REG_SZ,
			//	(const BYTE*)mongoPath.c_str(),
			//	mongoPath.size()

			//);
			//RegCloseKey(keyHandle);

			//HKEY hKey;
			//LPCSTR subKey = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment\\";
			//if (RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
			//	std::cerr << "Error opening key." << std::endl;
			//	return 1;
			//}
			//LONG result = RegSetValueA(
			//	HKEY_LOCAL_MACHINE, // Root key
			//	"SYSTEM\CurrentControlSet\Control\Session Manager\Environment",             // Subkey
			//	REG_SZ,
			//	m_MongoAdressEnvVar.c_str(),                  // lpValueName
			//	mongoPath.c_str(),
			//	mongoPath.size()

			//);
			//std::string msg = wuf::getErrorMessage(result);
		}
	}
	else
	{
		Logger::INSTANCE().write("Env var not found\n");
	}
}
