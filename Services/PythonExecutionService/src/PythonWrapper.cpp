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

void PythonWrapper::ExecuteFunction(const std::string& functionName, const std::string& moduleName)
{
	CPythonObjectBorrowed function(LoadFunction(functionName, moduleName)); //Really borrowed?
	CPythonObjectNew returnValue(PyObject_CallObject(function, nullptr));
}

void PythonWrapper::ExecuteFunction(const std::string& functionName, int64_t& outReturn, const std::string& moduleName)
{
	CPythonObjectBorrowed function(LoadFunction(functionName, moduleName)); //Really borrowed?
	CPythonObjectNew returnValue(PyObject_CallObject(function, nullptr));
	outReturn = getInt64Value(returnValue, "return value");
}

void PythonWrapper::ExecuteFunction(const std::string& functionName, double& outReturn, const std::string& moduleName)
{
	CPythonObjectBorrowed function(LoadFunction(functionName, moduleName)); //Really borrowed?
	CPythonObjectNew returnValue(PyObject_CallObject(function, nullptr));
	outReturn = getDoubleValue(returnValue, "return value");
}

void PythonWrapper::ExecuteFunction(const std::string& functionName, std::string& outReturn, const std::string& moduleName)
{
	CPythonObjectBorrowed function(LoadFunction(functionName, moduleName)); //Really borrowed?
	CPythonObjectNew returnValue(PyObject_CallObject(function, nullptr));
	outReturn = getStringValue(returnValue, "return value");
}

void PythonWrapper::ExecuteFunction(const std::string& functionName, bool& outReturn, const std::string& moduleName)
{
	CPythonObjectBorrowed function(LoadFunction(functionName, moduleName)); //Really borrowed?
	CPythonObjectNew returnValue(PyObject_CallObject(function, nullptr));
	outReturn = getBoolValue(returnValue, "return value");
}

PyObject* PythonWrapper::LoadFunction(const std::string& functionName, const std::string& moduleName)
{
	CPythonObjectBorrowed module(GetModule(moduleName));
	CPythonObjectBorrowed globalDirectory(PyModule_GetDict(module));
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


int64_t PythonWrapper::getInt64Value(PyObject* pValue, const std::string& varName)
{
	if (PyLong_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return PyLong_AsLong(pValue);
}

double PythonWrapper::getDoubleValue(PyObject* pValue, const std::string& varName)
{
	if (PyFloat_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return PyFloat_AsDouble(pValue);
}

std::string PythonWrapper::getStringValue(PyObject* pValue, const std::string& varName)
{
	if (PyUnicode_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return std::string(PyUnicode_AsUTF8(pValue));
}

bool PythonWrapper::getBoolValue(PyObject* pValue, const std::string& varName)
{
	if (PyBool_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return static_cast<bool>(PyLong_AsLong(pValue));
	
}

void PythonWrapper::GetGlobalVariableValue(const std::string& varName, int64_t& outReturn, const std::string& moduleName)
{
	CPythonObjectNew pythonVar(GetGlobalVariable(varName, moduleName));
	outReturn = getInt64Value(pythonVar, varName);
}


void PythonWrapper::GetGlobalVariableValue(const std::string& varName, double& outReturn, const std::string& moduleName)
{
	CPythonObjectNew pythonVar(GetGlobalVariable(varName, moduleName));
	outReturn = getDoubleValue(pythonVar, varName);
}

void PythonWrapper::GetGlobalVariableValue(const std::string& varName, std::string& outReturn, const std::string& moduleName)
{
	CPythonObjectNew pythonVar(GetGlobalVariable(varName, moduleName));

}

void PythonWrapper::GetGlobalVariableValue(const std::string& varName, bool outReturn, const std::string& moduleName)
{
	CPythonObjectNew pythonVar(GetGlobalVariable(varName, moduleName));
	
}

PyObject* PythonWrapper::GetModule(const std::string& moduleName)
{
	PyObject* module =	PyImport_ImportModule(moduleName.c_str());
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


PyObject* PythonWrapper::GetGlobalVariable(const std::string& varName, const std::string& moduleName)
{
	CPythonObjectBorrowed module(GetModule(moduleName));
	CPythonObjectBorrowed globalDirectory(PyModule_GetDict(module));
	PyObject* pythonVar = PyDict_GetItemString(globalDirectory, varName.c_str());
	if (pythonVar == nullptr)
	{
		ThrowPythonException();
	}
	return pythonVar;
}

