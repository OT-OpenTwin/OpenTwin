//! @brief  Functions that investigate an existing module for the starting function. If the module contains only one funtion, it will be the starting function of the module. 
//!			If the module holds multiple function definitions it requires one function called "__main__", which will be the starting function of the module.

#pragma once
#include <string>
#include "CPythonObject.h"
class PythonModuleAPI
{
public:
	static const PythonModuleAPI& instance(void);

	std::string getModuleEntryPoint(const std::string& _moduleName) const;
	std::string getModuleEntryPoint(CPythonObject& _module) const;

public:
	PythonModuleAPI() = default;
	std::string m_defaultEntryPoint = "__main__";

	bool hasScriptHasEntryPoint(const CPythonObject& _module) const;
};

