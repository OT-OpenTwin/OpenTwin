// @otlicense

#include "FixturePythonAPI.h"


void FixturePythonAPI::SetupModule(const std::string& script)
{
	_wrapper.execute(script, _moduleName);
}