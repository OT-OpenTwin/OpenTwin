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
			//PyErr_GivenExceptionMatches()
			/*
			*	PyExc_BaseException
				PyExc_Exception
				PyExc_ArithmeticError
				PyExc_AssertionError
				PyExc_AttributeError
				PyExc_BlockingIOError
				PyExc_BrokenPipeError
				PyExc_BufferError
				PyExc_ChildProcessError
				PyExc_ConnectionAbortedError
				PyExc_ConnectionError
				PyExc_ConnectionRefusedError
				PyExc_ConnectionResetError
				PyExc_EOFError
				PyExc_FileExistsError
				PyExc_FileNotFoundError
				PyExc_FloatingPointError
				PyExc_GeneratorExit
				PyExc_ImportError
				PyExc_IndentationError
				PyExc_IndexError
				PyExc_InterruptedError
				PyExc_IsADirectoryError
				PyExc_KeyError
				PyExc_KeyboardInterrupt
				PyExc_LookupError
				PyExc_MemoryError
				PyExc_ModuleNotFoundError
				PyExc_NameError
				PyExc_NotADirectoryError
				PyExc_NotImplementedError
				PyExc_OSError
				PyExc_OverflowError
				PyExc_PermissionError
				PyExc_ProcessLookupError
				PyExc_RecursionError
				PyExc_ReferenceError
				PyExc_RuntimeError
				PyExc_StopAsyncIteration
				PyExc_StopIteration
				PyExc_SyntaxError
				PyExc_SystemError
				PyExc_SystemExit
				PyExc_TabError
				PyExc_TimeoutError
				PyExc_TypeError
				PyExc_UnboundLocalError
				PyExc_UnicodeDecodeError
				PyExc_UnicodeEncodeError
				PyExc_UnicodeError
				PyExc_UnicodeTranslateError
				PyExc_ValueError
				PyExc_ZeroDivisionError
			*/


		std::string exceptionCause = PyUnicode_AsUTF8(errorMessagePython);
		if (exceptionCause != "<NULL>")
		{

			PyObject* errorTypeP = PyObject_Str(type);
			std::string errorType = PyUnicode_AsUTF8(errorTypeP);
			if (errorType.find("'") != std::string::npos)
			{
				errorType = errorType.substr(errorType.find_first_of('\'')+1, errorType.size());
				errorType = errorType.substr(0, errorType.find('\''));
			}
			message += "Python exception type " + errorType + " was thrown: ";
			message += exceptionCause;
		}
		Py_DECREF(errorMessagePython);
	}

	PythonException() : PythonException("") {}
	
	const char* what() const override
	{
		return message.c_str();
	};

private:
	std::string message;
};