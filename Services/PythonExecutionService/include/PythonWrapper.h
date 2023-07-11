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
#include <list>
#include "CPythonObjectBorrowed.h"
#include "CPythonObjectNew.h"
#include "PythonException.h"

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
		void ResetSysPath();
		void AddToSysPath(const std::string& newPathComponent);
		void ClosePythonInterpreter();

		void Execute(const std::string& executionCommand, const std::string& moduleName = "__main__");
		
		CPythonObjectNew ExecuteFunction(const std::string& functionName, CPythonObject& parameter, const std::string& moduleName = "__main__");
		
		CPythonObjectBorrowed GetGlobalVariable(const std::string& varName, const std::string& moduleName);

	private:
		std::list<std::string> _pythonPath;
		std::string _defaultSitePackagesPath;
		bool _interpreterSuccessfullyInitialized = false;

		static void signalHandlerAbort(int sig);
		
		PyObject* GetModule(const std::string& moduleName);
		PyObject* LoadFunction(const std::string& functionName, const std::string& moduleName = "__main__");
		
	};
