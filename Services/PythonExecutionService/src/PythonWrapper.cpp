#include "PythonWrapper.h"
#include <signal.h>
#include "PythonObjectBuilder.h"
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

void PythonWrapper::Execute(const std::string& executionCommand, const std::string& moduleName)
{
	CPythonObjectBorrowed module(GetModule(moduleName));
	CPythonObjectBorrowed globalDirectory(PyModule_GetDict(module));
	PyObject* result(PyRun_String(executionCommand.c_str(), Py_file_input, globalDirectory, globalDirectory));
	if (result == nullptr)
	{
		ThrowPythonException();
	}
}

CPythonObjectNew PythonWrapper::ExecuteFunction(const std::string& functionName, CPythonObject& parameter, const std::string& moduleName)
{
	CPythonObjectBorrowed function(LoadFunction(functionName, moduleName)); //Really borrowed?
	CPythonObjectNew returnValue(PyObject_CallObject(function, parameter));
	if (!returnValue.ReferenceIsSet())
	{
		ThrowPythonException();
	}
	return returnValue;
}


CPythonObjectBorrowed PythonWrapper::GetGlobalVariable(const std::string& varName, const std::string& moduleName)
{
	CPythonObjectBorrowed module(GetModule(moduleName));
	CPythonObjectBorrowed globalDirectory(PyModule_GetDict(module));
	CPythonObjectBorrowed pythonVar(PyDict_GetItemString(globalDirectory, varName.c_str()));
	if (pythonVar == nullptr)
	{
		ThrowPythonException();
	}
	return pythonVar;
}


PyObject* PythonWrapper::LoadFunction(const std::string& functionName, const std::string& moduleName)
{
	CPythonObjectBorrowed module(GetModule(moduleName));
	PyObject* function = PyObject_GetAttrString(module, functionName.c_str());
	if (function == nullptr)
	{
		ThrowPythonException();
	}
	if (PyCallable_Check(function) != 1)
	{
		throw std::exception(("Function " + functionName + " does not exist in module " + moduleName).c_str());
	}
	return function;
}


PyObject* PythonWrapper::GetModule(const std::string& moduleName)
{
	PyObject* module = PyImport_ImportModule(moduleName.c_str());
	if (module == nullptr)
	{
		if (moduleName == "__main__")
		{
			ThrowPythonException();
		}
		PyObject* type, * value, * traceback;
		PyErr_Fetch(&type, &value, &traceback);
		module = PyImport_AddModule(moduleName.c_str());
	}
	return module;	
}


