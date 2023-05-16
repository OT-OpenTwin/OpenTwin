#include "PythonWrapper.h"

PythonWrapper::PythonWrapper()
{
	_pythonRoot = getenv("PYTHON310_ROOT");
	_pythonPath = 
		_pythonRoot + "\\Lib;"+
		_pythonRoot + "\\Lib\\site-packages;"+
		_pythonRoot;
}

void PythonWrapper::ExecuteString(std::string executionCommand)
{
	try
	{
		if (!_pythonInterpreterIsInitialized)
		{
			InitializePythonInterpreter();
		}
		
		int start = Py_file_input;
		PyObject* main = PyImport_AddModule("__main__");
		PyObject* globalDictionary = PyModule_GetDict(main);
		PyObject* localDictionary = PyDict_New();

		std::string allParameter = GetAllGlobalParameter();

		PyObject* allParameterSet = PyRun_String(allParameter.c_str(), start, localDictionary, globalDictionary);

		if (allParameterSet == nullptr)
		{
			ThrowPythonException();
		}


		PyObject* returnVal = PyRun_String(executionCommand.c_str(), start, localDictionary, globalDictionary);
		if (returnVal == nullptr)
		{
			ThrowPythonException();
		}

		SetAllGlobalParameter(&globalDictionary);
		
		ClosePythonInterpreter();
	}
	catch (const std::exception& exception)
	{
		ClosePythonInterpreter();
		throw exception;
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
	_pythonInterpreterIsInitialized = true;
}

void PythonWrapper::operator<<(const std::string& executionCommand)
{
}

std::string PythonWrapper::GetAllGlobalParameter()
{
	std::string allParameter = "";
	AddParameterString(allParameter, _globalParameterBool);
	AddParameterString(allParameter, _globalParameterDouble);
	AddParameterString(allParameter, _globalParameterFloat);
	AddParameterString(allParameter, _globalParameterInt32);
	AddParameterString(allParameter, _globalParameterInt64);
	AddParameterString(allParameter, _globalParameterString);
	
	return allParameter;
}

void PythonWrapper::SetAllGlobalParameter(PyObject** globalDict)
{
	UpdateParameter(globalDict, _globalParameterBool);
	UpdateParameter(globalDict, _globalParameterDouble);
	UpdateParameter(globalDict, _globalParameterFloat);
	UpdateParameter(globalDict, _globalParameterInt32);
	UpdateParameter(globalDict, _globalParameterInt64);
	UpdateParameter(globalDict, _globalParameterString);
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

void PythonWrapper::ClosePythonInterpreter()
{
	int success = Py_FinalizeEx();
	if (success == -1)
	{
		ThrowPythonException();
	}
	_pythonInterpreterIsInitialized = false;
}
