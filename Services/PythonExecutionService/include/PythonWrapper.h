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
		PythonWrapper(const PythonWrapper& other) = delete;
		PythonWrapper& operator=(const PythonWrapper& other) = delete;
		PythonWrapper(const PythonWrapper&& other) = delete;
		PythonWrapper& operator=(const PythonWrapper&& other) = delete;

		~PythonWrapper();
		
		void InitializePythonInterpreter();
		void ClosePythonInterpreter();

		void ExecuteString(const std::string& executionCommand, const std::string& moduleName = "__main__");
		//void ExecuteString(const std::string& executionCommand, int& outReturn, const std::string& moduleName = "__main__");

		//void ExecuteFunction(const std::string& functionName, const std::string& moduleName = "__main__");
		//void ExecuteFunction(const std::string& functionName, int& outReturn, const std::string& moduleName = "__main__");

		//void ExtractGlobalVariables(ot::VariableBundle& outGlobalVariables, const std::string& moduleName = "__main__");

	private:
		std::string _pythonPath;
		bool _interpreterSuccessfullyInitialized = false;

		void ThrowPythonException();
		static void signalHandlerAbort(int sig);
		
		PyObject* GetModule(const std::string& moduleName);

		//void ExtractValueFromGlobalDirectory(ot::Variable<std::string>& variable, PyObject* activeGlobalDirectory);
		//void ExtractValueFromGlobalDirectory(ot::Variable<double>& variable, PyObject* activeGlobalDirectory);
		//void ExtractValueFromGlobalDirectory(ot::Variable<float>& variable, PyObject* activeGlobalDirectory);
		//void ExtractValueFromGlobalDirectory(ot::Variable<int32_t>& variable, PyObject* activeGlobalDirectory);
		//void ExtractValueFromGlobalDirectory(ot::Variable<int64_t>& variable, PyObject* activeGlobalDirectory);
		//void ExtractValueFromGlobalDirectory(ot::Variable<bool>& variable, PyObject* activeGlobalDirectory);
	};
