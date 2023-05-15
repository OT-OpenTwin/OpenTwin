#pragma once
#define PY_SSIZE_T_CLEAN // # 
#include <Python.h>
#include <string>

class PythonWrapper
{
public:
	PythonWrapper();
	void ExecuteString(std::string executionCommand);

private:
	std::string _pythonPath;
	std::string _pythonRoot;

	void ThrowPythonException();
	void ClosePythonInterpreter();
};
