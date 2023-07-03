#include "PythonAPI.h"

PythonAPI::PythonAPI()
{
	wrapper.InitializePythonInterpreter();
}

void PythonAPI::InterpreteString(const std::string& programm, ot::VariableBundle& variables)
{
	/*wrapper.ExecuteString(programm, variables);*/
}

void PythonAPI::InterpreteString(std::vector<std::string>& programms, std::vector<ot::VariableBundle>& variables)
{
	//auto variableSet = variables.begin();
	//wrapper.InitiateExecutionSequence();
	//PyObject* activeDirectory = PyDict_New();
	//for (const std::string programm : programms)
	//{
	//	wrapper.ExecuteString(programm, activeDirectory);
	//	//wrapper << programm;
	//	if (variableSet != variables.end())
	//	{
	//		wrapper.ExtractVariables(*variableSet, activeDirectory);
	//		variableSet++;
	//	}
	//}
	//wrapper.EndExecutionSequence();
}
