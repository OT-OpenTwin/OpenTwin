#include "FixturePythonWrapper.h"
#include <tuple>
#include <vector>

FixturePythonWrapper::FixturePythonWrapper()
{
}

PythonWrapper* FixturePythonWrapper::getPythonWrapper()
{
	return &pythonWrapper;
}

std::string FixturePythonWrapper::getAllGlobalParameter()
{
	return pythonWrapper.GetAllGlobalParameter();
}

void FixturePythonWrapper::SetFalsePythonSysPath()
{
	pythonWrapper._pythonPath = "C:\\Users";
}
