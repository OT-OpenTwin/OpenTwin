/*****************************************************************//**
 * \file   PythonObjectBuilder.h
 * \brief  Builder to transfer typically used PyObjects to c++ types and vice versa.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once
#include <Python.h>
#include "CPythonObjectNew.h"
#include "CPythonObjectBorrowed.h"
#include "OTCore/Variable.h"
#include "OTCore/JSON.h"
#include "OTCore/Variable.h"
#include "OTCore/GenericDataStruct.h"

#include <stdint.h>
#include <string>
#include <list>
#include <map>
#include <memory>
#include <optional>

/// <summary>
/// PyObjects don't distingiush between float and double and int32 and int64. All floating point values are handled as double and all integer values are handled as int64
/// </summary>
class PythonObjectBuilder
{
public:
	PythonObjectBuilder() : _assembly(nullptr) {};
	~PythonObjectBuilder() { Py_XDECREF(_assembly); };
	void StartTupleAssemply(int size);
	void operator<<(CPythonObject* newEntry);
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

	ot::GenericDataStructList getGenericDataStructList(CPythonObject& pValue);
	ot::GenericDataStruct* getGenericDataStruct(CPythonObject& pValue);
	std::list<ot::Variable> getVariableList(CPythonObject& pValue);
	std::optional<ot::Variable> getVariable(CPythonObject& pValue);

	CPythonObjectNew setInt32(const int32_t value);
	CPythonObjectNew setInt64(const int64_t value);
	CPythonObjectNew setDouble(double value);
	CPythonObjectNew setString(const std::string& value);
	CPythonObjectNew setBool(const bool value);

	CPythonObjectNew setVariableTuple(const std::list<ot::Variable>& values);
	CPythonObjectNew setVariableList(const std::list<ot::Variable>& values);
	CPythonObjectNew setVariableList(const std::vector<ot::Variable>& values);
	CPythonObjectNew setVariableList(rapidjson::GenericArray<false,rapidjson::Value> & values);
	
	CPythonObjectNew setVariable(const ot::Variable& value);

	CPythonObjectNew setGenericDataStruct(ot::GenericDataStruct* genericDataStruct);
	CPythonObjectNew setGenericDataStructList(const ot::GenericDataStructList& values);

private:
	PyObject* _assembly = nullptr;
	int _assemblySize = 0;
	int _currentSize = 0;
};


