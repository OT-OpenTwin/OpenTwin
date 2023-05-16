#pragma once
#define PY_SSIZE_T_CLEAN // # 
#include <Python.h>
#include <string>
#include <vector>
#include <signal.h>

#include "PythonParameter.h"

	class  PythonWrapper
	{
		friend class FixturePythonWrapper;
	public:
		PythonWrapper();
		void ExecuteString(std::string executionCommand);
		void InitializePythonInterpreter();
		void ClosePythonInterpreter();

		void operator<<(const std::string& executionCommand);

		void setGlobalParameter(std::vector<PythonAPI::PythonParameter<int32_t>>* parameterList) { _globalParameterInt32 = parameterList; };
		void setGlobalParameter(std::vector<PythonAPI::PythonParameter<int64_t>>* parameterList) { _globalParameterInt64 = parameterList; };
		void setGlobalParameter(std::vector<PythonAPI::PythonParameter<std::string>>* parameterList) { _globalParameterString = parameterList; };
		void setGlobalParameter(std::vector<PythonAPI::PythonParameter<double>>* parameterList) { _globalParameterDouble = parameterList; };
		void setGlobalParameter(std::vector<PythonAPI::PythonParameter<float>>* parameterList) { _globalParameterFloat = parameterList; };
		void setGlobalParameter(std::vector<PythonAPI::PythonParameter<bool>>* parameterList) { _globalParameterBool = parameterList; };

	private:
		std::string _pythonPath;
		std::string _pythonRoot;
		bool _pythonInterpreterIsInitialized;

		std::vector<PythonAPI::PythonParameter<int32_t>>* _globalParameterInt32;
		std::vector<PythonAPI::PythonParameter<int64_t>>* _globalParameterInt64;
		std::vector<PythonAPI::PythonParameter<std::string>>* _globalParameterString;
		std::vector<PythonAPI::PythonParameter<double>>* _globalParameterDouble;
		std::vector<PythonAPI::PythonParameter<float>>* _globalParameterFloat;
		std::vector<PythonAPI::PythonParameter<bool>>* _globalParameterBool;

		std::string GetAllGlobalParameter();
		void SetAllGlobalParameter(PyObject* globalDict);
		void ThrowPythonException();

		static void SignalHandlerForPythonCrash(int signum)
		{
			throw std::exception("Signal for exiting the application was raised.");
		}
	};