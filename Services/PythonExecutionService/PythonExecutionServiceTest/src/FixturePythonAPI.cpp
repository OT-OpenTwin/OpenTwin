#include "FixturePythonAPI.h"


void FixturePythonAPI::SetupModule(const std::string& script)
{
	_api._wrapper.Execute(script, _moduleName);
}