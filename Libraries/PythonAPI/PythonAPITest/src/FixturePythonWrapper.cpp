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

void FixturePythonWrapper::SetFalsePythonSysPath()
{
	pythonWrapper._pythonPath = "C:\\Users";
}

void FixturePythonWrapper::CleanDictionaryCopy()
{
	pythonWrapper._cleanGlobalDirectory.reset(PyModule_GetDict(pythonWrapper._mainModule));
	PyObject* copy = PyDict_Copy(pythonWrapper._cleanGlobalDirectory);
	pythonWrapper.InitiateExecutionSequence();
	pythonWrapper<< "print(\"HEllo\")";
	PyObject* copy2 = PyDict_Copy(pythonWrapper._cleanGlobalDirectory);
	PyObject* copy3 = PyDict_Copy(pythonWrapper._cleanGlobalDirectory);
}
