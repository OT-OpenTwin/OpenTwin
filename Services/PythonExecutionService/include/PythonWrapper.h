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

		void Execute(const std::string& executionCommand, const std::string& moduleName = "__main__");
		
		void ExecuteFunction(const std::string& functionName, const std::string& moduleName = "__main__");
		void ExecuteFunction(const std::string& functionName, int64_t& outReturn, const std::string& moduleName = "__main__");
		void ExecuteFunction(const std::string& functionName, double& outReturn, const std::string& moduleName = "__main__");
		void ExecuteFunction(const std::string& functionName, std::string& outReturn, const std::string& moduleName = "__main__");
		void ExecuteFunction(const std::string& functionName, bool& outReturn, const std::string& moduleName = "__main__");

		void GetGlobalVariableValue(const std::string& varName, int64_t& outReturn, const std::string& moduleName = "__main__");
		void GetGlobalVariableValue(const std::string& varName, double& outReturn, const std::string& moduleName = "__main__");
		void GetGlobalVariableValue(const std::string& varName, std::string& outReturn, const std::string& moduleName = "__main__");
		void GetGlobalVariableValue(const std::string& varName, bool outReturn, const std::string& moduleName = "__main__");

		void StartExecutionSequence(const std::string& moduleName = "__main__");
		void EndExecutionSequence();

	private:
		std::string _pythonPath;
		bool _interpreterSuccessfullyInitialized = false;
		PyObject* _openedModuleGlobalDictionary = nullptr;
		PyObject* _openedModule = nullptr;


		void ThrowPythonException();
		static void signalHandlerAbort(int sig);
		
		PyObject* GetModule(const std::string& moduleName);
		PyObject* GetGlobalVariable(const std::string& varName, const std::string& moduleName);
		PyObject* LoadFunction(const std::string& functionName, const std::string& moduleName = "__main__");
		
		inline int64_t getInt64Value(PyObject* pValue, const std::string& varName);
		inline double getDoubleValue(PyObject* pValue, const std::string& varName);
		inline std::string getStringValue(PyObject* pValue, const std::string& varName);
		inline bool getBoolValue(PyObject* pValue, const std::string& varName);

	};
