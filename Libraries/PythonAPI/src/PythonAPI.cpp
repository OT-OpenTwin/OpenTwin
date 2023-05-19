#include "PythonAPI.h"

PythonAPI::PythonAPI()
{
	wrapper.InitializePythonInterpreter();
}

void PythonAPI::InterpreteString(const std::string& programm, ot::VariableBundle& variables)
{
	wrapper.ExecuteString(programm, variables);
}

void PythonAPI::InterpreteString(std::vector<std::string>& programms, std::vector<ot::VariableBundle>& variables)
{
	auto variableSet = variables.begin();
	wrapper.InitiateExecutionSequence();
	for (const std::string programm : programms)
	{
		wrapper << programm;
		wrapper.ExtractVariables(*variableSet);
		variableSet++;
	}
	wrapper.EndExecutionSequence();
}
