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

		std::vector<PythonAPI::PythonParameter<int32_t>>* _globalParameterInt32 = nullptr;
		std::vector<PythonAPI::PythonParameter<int64_t>>* _globalParameterInt64 = nullptr;
		std::vector<PythonAPI::PythonParameter<std::string>>* _globalParameterString = nullptr;
		std::vector<PythonAPI::PythonParameter<double>>* _globalParameterDouble = nullptr;
		std::vector<PythonAPI::PythonParameter<float>>* _globalParameterFloat = nullptr;
		std::vector<PythonAPI::PythonParameter<bool>>* _globalParameterBool = nullptr;

		std::string GetAllGlobalParameter();
		void SetAllGlobalParameter(PyObject** globalDict);
		void ThrowPythonException();

		template <class T>
		void AddParameterString(std::string& totalString, std::vector<PythonAPI::PythonParameter<T>>* parameter);
		template <class T>
		void UpdateParameter(PyObject** globalDict, std::vector<PythonAPI::PythonParameter<T>>* parameter);
	};

	template<class T>
	inline void PythonWrapper::AddParameterString(std::string& totalString, std::vector<PythonAPI::PythonParameter<T>>* parameters)
	{
		if (parameters != nullptr)
		{
			for (const auto& parameter : *parameters)
			{
				totalString += parameter.getParameterString() + "\n";
			}
		}
	}

	template<class T>
	inline void PythonWrapper::UpdateParameter(PyObject** globalDict, std::vector<PythonAPI::PythonParameter<T>>* parameters)
	{
		if (parameters != nullptr)
		{
			for (auto& parameter : *parameters)
			{
				parameter.getValueFromDictionary(globalDict);
			}
		}
	}
