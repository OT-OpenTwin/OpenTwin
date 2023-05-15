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
		std::wstring pyhtonPath(_pythonPath.begin(), _pythonPath.end());
		int start = Py_file_input;
		Py_SetPath(pyhtonPath.c_str());
		int skipSignalHandlerRegistration = 0; //0: Skips; 1: not skipping (equivalent to Py_Initialize())
		Py_InitializeEx(skipSignalHandlerRegistration);

		PyObject* main = PyImport_AddModule("__main__");
		PyObject* globalDictionary = PyModule_GetDict(main);
		PyObject* localDictionary = PyDict_New();

		PyObject* returnVal = PyRun_String(executionCommand.c_str(), start, globalDictionary, localDictionary);

		if (returnVal == nullptr)
		{
			ThrowPythonException();
		}
	}
	catch (const std::exception& exception)
	{
		ClosePythonInterpreter();
		throw exception;
	}
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
}
