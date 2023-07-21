#pragma once
#include <string>
#include "CPythonObject.h"
class PythonModuleAPI
{
public:
	static std::string GetModuleEntryPoint(const std::string& moduleName);
	static std::string GetModuleEntryPoint(CPythonObject& module);
};
