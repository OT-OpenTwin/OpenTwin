#include "FixturePythonAPI.h"


void FixturePythonAPI::SetupModule(const std::string& script)
{
	_wrapper.Execute(script, _moduleName);
}