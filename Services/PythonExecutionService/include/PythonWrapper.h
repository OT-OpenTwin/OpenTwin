/*****************************************************************//**
 * \file   PythonWrapper.h
 * \brief  Wrapper around python code execution.
 * 
 * \author Wagner
 * \date   May 2023
 *********************************************************************/

#pragma once
#define PY_SSIZE_T_CLEAN // # 
#include <Python.h>
#include <string>
#include <vector>

#include "CPythonObjectBorrowed.h"
#include "CPythonObjectNew.h"

#include "OpenTwinCore/Variable.h"

	class  PythonWrapper
	{
		friend class FixturePythonWrapper;
	public:
		PythonWrapper();
		~PythonWrapper();
		void InitializePythonInterpreter();
		void ClosePythonInterpreter();

		void ExecuteString(std::string executionCommand, ot::VariableBundle& globalVariables);
		void ExecuteString(std::string executionCommand, PyObject* activeDirectory);
		void operator<<(const std::string& executionCommand);

		void ExtractVariables(ot::VariableBundle& globalVariables, PyObject* activeGlobalDirectory);
		void ExtractVariables(ot::VariableBundle& globalVariables);
		void InitiateExecutionSequence();
		void EndExecutionSequence();

	private:
		std::string _pythonPath;
		std::string _pythonRoot;

		CPythonObjectNew _mainModule = nullptr;
		CPythonObjectBorrowed _cleanGlobalDirectory = nullptr;
		CPythonObjectNew _activeGlobalDirectory = nullptr;

		void ThrowPythonException();
		void ExtractValueFromGlobalDirectory(ot::Variable<std::string>& variable, PyObject* activeGlobalDirectory);
		void ExtractValueFromGlobalDirectory(ot::Variable<double>& variable, PyObject* activeGlobalDirectory);
		void ExtractValueFromGlobalDirectory(ot::Variable<float>& variable, PyObject* activeGlobalDirectory);
		void ExtractValueFromGlobalDirectory(ot::Variable<int32_t>& variable, PyObject* activeGlobalDirectory);
		void ExtractValueFromGlobalDirectory(ot::Variable<int64_t>& variable, PyObject* activeGlobalDirectory);
		void ExtractValueFromGlobalDirectory(ot::Variable<bool>& variable, PyObject* activeGlobalDirectory);
	};
