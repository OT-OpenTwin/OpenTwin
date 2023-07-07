#pragma once
#include <exception>
#include <Python.h>
#include <string>

class PythonException : public std::exception
{
public:
	PythonException(const std::string& msg) : message(msg) 
	{
		PyObject* type, * value, * traceback;
		PyErr_Fetch(&type, &value, &traceback);
		PyErr_NormalizeException(&type, &value, &traceback);
		PyObject* errorMessagePython = PyObject_Str(value);
		message += PyUnicode_AsUTF8(errorMessagePython);
		Py_DECREF(errorMessagePython);
	}

	PythonException() : PythonException("") {}
	
	char* what() 
	{
		throw std::exception( message.c_str());
	};

private:
	std::string message;
};