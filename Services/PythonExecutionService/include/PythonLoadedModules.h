#pragma once
#include <map>
#include <string>

class PythonLoadedModules
{
public:
	static PythonLoadedModules* INSTANCE()
	{
		static PythonLoadedModules instance;
		return &instance;
	}
	
	std::map<std::string, std::string>* getModuleNamesByScriptName() {return &_moduleNamesByScriptName; }

private:
	PythonLoadedModules() {};
	std::map<std::string, std::string> _moduleNamesByScriptName;

};
