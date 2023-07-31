/*****************************************************************//**
 * \file   PythonModuleAPI.h
 * \brief  Functions that investigate an existing module for the starting function. If the module contains only one funtion, it will be the starting function of the module.
 *			If the module holds multiple function definitions it requires one function called "__main__", which will be the starting function of the module.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once
#include <string>
#include "CPythonObject.h"
class PythonModuleAPI
{
public:
	static std::string GetModuleEntryPoint(const std::string& moduleName);
	static std::string GetModuleEntryPoint(CPythonObject& module);
};
