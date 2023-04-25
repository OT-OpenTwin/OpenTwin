#pragma once
#include <map>
#include <string>

namespace UserAPI {
	__declspec(dllexport) std::map<int, std::string>  getAllUsers();
	__declspec(dllexport) std::string  getUser(int);

	__declspec(dllexport) std::map<int, std::string>  saveUser(std::string);
	__declspec(dllexport) bool updateUser(int, std::string);
	__declspec(dllexport) std::map<int, std::string> deleteUser(int);
}


