#pragma once
#include <string>

class PythonModuleAPI
{
public:
	std::string GetModuleEntryPoint(const std::string& moduleName);
};
