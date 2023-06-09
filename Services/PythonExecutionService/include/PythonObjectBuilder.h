#pragma once
#include <Python.h>
#include "CPythonObjectNew.h"
#include "CPythonObjectBorrowed.h"

#include <stdint.h>
#include <string>
#include <list>
#include <map>
#include <variant>

using variable_t = std::variant<int32_t, int64_t, bool, float, double, const char*>;


class PythonObjectBuilder
{
public:

	PythonObjectBuilder() : _assembly(nullptr) {};

	void StartTupleAssemply(int size);
	void operator<<(CPythonObject&& newEntry);
	CPythonObjectNew getAssembledTuple();

	int32_t getInt32Value(const CPythonObject& pValue, const std::string& varName);
	double getDoubleValue(const CPythonObject& pValue, const std::string& varName);
	std::string getStringValue(const CPythonObject& pValue, const std::string& varName);
	bool getBoolValue(const CPythonObject& pValue, const std::string& varName);

	CPythonObjectBorrowed getTupleItem(const CPythonObject& pValue, int position, const std::string& varName);
	int32_t getInt32ValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName);
	double getDoubleValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName);
	std::string getStringValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName);
	bool getBoolValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName);

	CPythonObjectBorrowed getDictItem(const CPythonObject& pValue);
	
	CPythonObjectBorrowed getListItem(const CPythonObject& pValue, int position);
	std::list<int32_t> getInt32List(const CPythonObject& pValue, const std::string& varName);
	std::list<double> getDoubleList(const CPythonObject& pValue, const std::string& varName);
	std::list<std::string> getStringList(const CPythonObject& pValue, const std::string& varName);
	std::list<bool> getBoolList(const CPythonObject& pValue, const std::string& varName);

	//std::map<std::string,std::list<variable_t>> getVariable(CPythonObject& pValue);
	variable_t getVariable(CPythonObject& pValue);

	CPythonObjectNew setInt32(const int32_t value);
	CPythonObjectNew setDouble(double value);
	CPythonObjectNew setString(const std::string& value);
	CPythonObjectNew setBool(const bool value);

private:
	
	CPythonObjectNew _assembly;
	int _assemblySize = 0;
	int _currentSize = 0;
};
