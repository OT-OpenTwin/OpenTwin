// @otlicense

#include "PythonException.h"
#include <Python.h>

PythonException::PythonException() 
	: PythonException("")
{

}

PythonException::PythonException(const std::string& _msg)
	: m_message(_msg)
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
	if (exceptionCause != "<NULL>") {

		PyObject* errorTypeP = PyObject_Str(type);
		std::string errorType = PyUnicode_AsUTF8(errorTypeP);
		if (errorType.find("'") != std::string::npos) {
			errorType = errorType.substr(errorType.find_first_of('\'') + 1, errorType.size());
			errorType = errorType.substr(0, errorType.find('\''));
		}
		m_message += "Python exception type " + errorType + " was thrown: ";
		m_message += exceptionCause;
	}
	Py_DECREF(errorMessagePython);
}

const char* PythonException::what(void) const {
	return m_message.c_str();
}
