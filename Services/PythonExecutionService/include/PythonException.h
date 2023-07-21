/*****************************************************************//**
 * \file   PythonException.h
 * \brief  Embedded python functions don't throw exceptions. If the return value holds an error code that says that the function failed in execution, it is possible to 
 *			extract an error description. Actually it is mandatory to extract the error description, since the internal error flag is reset by doing so. If the error flag 
 *			is still active in the nextt execution of an embedded python function it will fail.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
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
		throw std::exception(message.c_str());
	};

private:
	std::string message;
};