#include "FixturePythonWrapper.h"

FixturePythonWrapper::FixturePythonWrapper()
{
	wrapper.InitializePythonInterpreter();
}

void FixturePythonWrapper::ExecuteString(const std::string& command)
{
	wrapper.ExecuteString(command);
}
