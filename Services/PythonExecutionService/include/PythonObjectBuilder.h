#pragma once
#include <Python.h>
#include "CPythonObjectNew.h"
#include <stdint.h>
#include <string>

class PythonObjectBuilder
{
public:
	static PythonObjectBuilder INSTANCE()
	{
		static PythonObjectBuilder instance;
		return instance;
	}

	int64_t getInt64Value(CPythonObject& pValue, const std::string& varName);
	double getDoubleValue(CPythonObject& pValue, const std::string& varName);
	std::string getStringValue(CPythonObject& pValue, const std::string& varName);
	bool getBoolValue(CPythonObject& pValue, const std::string& varName);

	CPythonObjectNew setInt32(const int32_t value);
	CPythonObjectNew setDouble(const double value);
	CPythonObjectNew setString(const std::string& value);
	CPythonObjectNew setBool(const bool value);
private:
	PythonObjectBuilder() {};
};
