#include "PythonWrapper.h"

#include <signal.h>

PythonWrapper::PythonWrapper()
{
	std::string pythonRoot;
#ifdef _RELEASEDEBUG
	pythonRoot = getenv("PYTHON310_ROOT");
	pythonRoot += "\\Lib";
#else
	pythonRoot = ".\\Python";
#endif // RELEASEDEBUG
	_pythonPath =
		pythonRoot + ";" +
		pythonRoot + "\\site-packages;" +
		pythonRoot;
	signal(SIGABRT, &signalHandlerAbort);
}

PythonWrapper::~PythonWrapper()
{
	if (!_interpreterSuccessfullyInitialized)
	{
		ClosePythonInterpreter();
	}
}

void PythonWrapper::ClosePythonInterpreter()
{
	int success = Py_FinalizeEx();
	if (success == -1)
	{
		ThrowPythonException();
	}
}

void PythonWrapper::InitializePythonInterpreter()
{
	std::wstring pyhtonPath(_pythonPath.begin(), _pythonPath.end());

	Py_SetPath(pyhtonPath.c_str());
	int skipSignalHandlerRegistration = 0; //0: Skips; 1: not skipping (equivalent to Py_Initialize())
	Py_InitializeEx(skipSignalHandlerRegistration);
	if (Py_IsInitialized() != 1)
	{
		ThrowPythonException();
	}
	_interpreterSuccessfullyInitialized = true;
}

void PythonWrapper::signalHandlerAbort(int sig)
{
	signal(SIGABRT, &signalHandlerAbort);
	throw std::exception("Abort was called.");
}

void PythonWrapper::ThrowPythonException()
{
	PyObject* type, * value, * traceback;
	PyErr_Fetch(&type, &value, &traceback);
	PyErr_NormalizeException(&type, &value, &traceback);
	PyObject* errorMessagePython = PyObject_Str(value);
	std::string errorMessage = PyUnicode_AsUTF8(errorMessagePython);
	Py_DECREF(errorMessagePython);
	throw std::exception(errorMessage.c_str());
}

void PythonWrapper::ExecuteString(const std::string& executionCommand, const std::string& moduleName)
{
	CPythonObjectBorrowed module(GetModule(moduleName));
	CPythonObjectBorrowed globalDirectory (PyModule_GetDict(module));
	CPythonObjectNew result(PyRun_String(executionCommand.c_str(), Py_file_input, globalDirectory, globalDirectory));
	if (result == nullptr)
	{
		ThrowPythonException();
	}
}

PyObject* PythonWrapper::GetModule(const std::string& moduleName)
{
	PyObject* module =	PyImport_ImportModule(moduleName.c_str());
	if (module == nullptr)
	{
		if (moduleName == "__main__")
		{
			throw std::exception("Main module could not be found");
		}
		module = PyImport_AddModule(moduleName.c_str());
	}
	return module;
}


//void PythonWrapper::ExtractValueFromGlobalDirectory(ot::Variable<std::string>& variable, PyObject* activeGlobalDirectory)
//{
//	CPythonObjectNew var = PyDict_GetItemString(activeGlobalDirectory, variable.name.c_str());
//	if (!var.ReferenceIsSet())
//	{
//		throw std::exception(("Failed to extract python variable " + variable.name + " from dictionary.").c_str());
//	}
//	if (PyUnicode_Check(var) == 1)
//	{
//		variable.value = std::string(PyUnicode_AsUTF8(var));
//	}
//	else
//	{
//		throw std::exception(("Requested python parameter " + variable.name + " is not an unicode type.").c_str());
//	}
//}
//
//void PythonWrapper::ExtractValueFromGlobalDirectory(ot::Variable<double>& variable, PyObject* activeGlobalDirectory)
//{
//	CPythonObjectNew var = PyDict_GetItemString(activeGlobalDirectory, variable.name.c_str());
//	if (!var.ReferenceIsSet())
//	{
//		throw std::exception(("Failed to extract python variable " + variable.name + " from dictionary.").c_str());
//	}
//	if (PyUnicode_Check(var) == 1)
//	{
//		variable.value = PyFloat_AsDouble(var);
//	}
//	else
//	{
//		throw std::exception(("Requested python parameter " + variable.name + " is not an floating type.").c_str());
//	}
//}
//
//void PythonWrapper::ExtractValueFromGlobalDirectory(ot::Variable<float>& variable, PyObject* activeGlobalDirectory)
//{
//	CPythonObjectNew var = PyDict_GetItemString(activeGlobalDirectory, variable.name.c_str());
//	if (!var.ReferenceIsSet())
//	{
//		throw std::exception(("Failed to extract python variable " + variable.name + " from dictionary.").c_str());
//	}
//	if (PyFloat_Check(var) == 1)
//	{
//		variable.value = (static_cast<float>(PyFloat_AsDouble(var)));
//	}
//	else
//	{
//		throw std::exception(("Requested python parameter " + variable.name + " is not an floating type.").c_str());
//	}
//}
//
//void PythonWrapper::ExtractValueFromGlobalDirectory(ot::Variable<int32_t>& variable, PyObject* activeGlobalDirectory)
//{
//	CPythonObjectNew var = PyDict_GetItemString(activeGlobalDirectory, variable.name.c_str());
//	if (!var.ReferenceIsSet())
//	{
//		throw std::exception(("Failed to extract python variable " + variable.name + " from dictionary.").c_str());
//	}
//	if (PyLong_Check(var) == 1)
//	{
//		variable.value = static_cast<int32_t>(PyLong_AsLong(var));
//	}
//	else
//	{
//		throw std::exception(("Requested python parameter " + variable.name + " is not an integer type.").c_str());
//	}
//}
//
//void PythonWrapper::ExtractValueFromGlobalDirectory(ot::Variable<int64_t>& variable, PyObject* activeGlobalDirectory)
//{
//	CPythonObjectNew var = PyDict_GetItemString(activeGlobalDirectory, variable.name.c_str());
//	if (!var.ReferenceIsSet())
//	{
//		throw std::exception(("Failed to extract python variable " + variable.name + " from dictionary.").c_str());
//	}
//	if (PyLong_Check(var) == 1)
//	{
//		variable.value = static_cast<int64_t>(PyLong_AsLongLong(var));
//	}
//	else
//	{
//		throw std::exception(("Requested python parameter " + variable.name + " is not an integer type.").c_str());
//	}
//}
//
//void PythonWrapper::ExtractValueFromGlobalDirectory(ot::Variable<bool>& variable, PyObject* activeGlobalDirectory)
//{
//	CPythonObjectNew var = PyDict_GetItemString(activeGlobalDirectory, variable.name.c_str());
//	if (!var.ReferenceIsSet())
//	{
//		throw std::exception(("Failed to extract python variable " + variable.name + " from dictionary.").c_str());
//	}
//	if (PyBool_Check(var) == 1)
//	{
//		variable.value = PyLong_AsLong(var);
//	}
//	else
//	{
//		throw std::exception(("Requested python parameter " + variable.name + " is not an unicode type.").c_str());
//	}
//}

//void PythonWrapper::ExtractVariables(ot::VariableBundle& globalVariables, PyObject* activeGlobalDirectory)
//{
//	auto boolVariables = globalVariables.GetVariablesBool();
//	for(int i = 0; i < boolVariables->size(); i++)
//	{
//		ExtractValueFromGlobalDirectory((*boolVariables)[i],activeGlobalDirectory);
//	}
//	
//	auto stringVariables = globalVariables.GetVariablesString();
//	for (int i = 0; i < stringVariables->size(); i++)
//	{
//		ExtractValueFromGlobalDirectory((*stringVariables)[i], activeGlobalDirectory);
//	}
//
//	auto doubleVariables = globalVariables.GetVariablesDouble();
//	for(int i = 0; i < doubleVariables->size(); i++)
//	{
//		ExtractValueFromGlobalDirectory((*doubleVariables)[i], activeGlobalDirectory);
//	}
//
//	auto floatVariables = globalVariables.GetVariablesFloat();
//	for (int i = 0; i < floatVariables->size(); i++)
//	{
//		ExtractValueFromGlobalDirectory((*floatVariables)[i], activeGlobalDirectory);
//	}
//	
//	auto int32Variables = globalVariables.GetVariablesInt32();
//	for (int i = 0; i < int32Variables->size(); i++)
//	{
//		ExtractValueFromGlobalDirectory((*int32Variables)[i], activeGlobalDirectory);
//	}
//
//	auto int64Variables = globalVariables.GetVariablesInt64();
//	for (int i = 0; i < int64Variables->size(); i++)
//	{
//		ExtractValueFromGlobalDirectory((*int64Variables)[i], activeGlobalDirectory);
//	}
//}
//
//void PythonWrapper::ExtractVariables(ot::VariableBundle& globalVariables)
//{
//	assert(_activeGlobalDirectory.ReferenceIsSet());
//	ExtractVariables(globalVariables, _cleanGlobalDirectory);
//}
//
