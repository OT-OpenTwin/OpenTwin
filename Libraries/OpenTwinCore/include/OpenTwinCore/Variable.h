#pragma once
#include <string>
#include <vector>
#include <stdint.h>

namespace ot
{
	template <class T>
	struct Variable
	{
		Variable(std::string name, std::string type) :name(name), type(type) {};
		Variable(std::string name, std::string type, T value) :name(name), type(type), value(value) {};
		std::string name;
		std::string type;
		T value;
	};

	class VariableBundle
	{
	public:
		void AddVariable(Variable<std::string> variable) { variablesString.push_back(variable); };
		void AddVariable(Variable<int32_t> variable) { variablesInt32.push_back(variable); };
		void AddVariable(Variable <int64_t> variable) { variablesInt64.push_back(variable); };
		void AddVariable(Variable<double> variable) { variablesDouble.push_back(variable); };
		void AddVariable(Variable<float> variable) { variablesFloat.push_back(variable); };
		void AddVariable(Variable<bool> variable) { variablesBool.push_back(variable); };

		std::list<Variable<std::string>>* GetVariablesString() { return &variablesString; }
		std::list<Variable<int32_t>>* GetVariablesInt32() { return &variablesInt32; };
		std::list<Variable<int64_t>>* GetVariablesInt64() { return &variablesInt64; };
		std::list<Variable<double>>* GetVariablesDouble() { return &variablesDouble; };
		std::list<Variable<float>>* GetVariablesFloat() { return &variablesFloat; };
		std::list<Variable<bool>>* GetVariablesBool() { return &variablesBool; };

	private:
		std::list<Variable<std::string>> variablesString;
		std::list<Variable<int32_t>> variablesInt32;
		std::list<Variable<int64_t>> variablesInt64;
		std::list<Variable<double>> variablesDouble;
		std::list<Variable<float>> variablesFloat;
		std::list<Variable<bool>> variablesBool;
	};
}