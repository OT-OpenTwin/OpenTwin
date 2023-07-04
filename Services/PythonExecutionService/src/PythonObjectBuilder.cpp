#include "PythonObjectBuilder.h"

int64_t PythonObjectBuilder::getInt64Value(CPythonObject& pValue, const std::string& varName)
{
	if (PyLong_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return PyLong_AsLong(pValue);
}

double PythonObjectBuilder::getDoubleValue(CPythonObject& pValue, const std::string& varName)
{
	if (PyFloat_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return PyFloat_AsDouble(pValue);
}

std::string PythonObjectBuilder::getStringValue(CPythonObject& pValue, const std::string& varName)
{
	if (PyUnicode_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return std::string(PyUnicode_AsUTF8(pValue));
}

bool PythonObjectBuilder::getBoolValue(CPythonObject& pValue, const std::string& varName)
{
	if (PyBool_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return static_cast<bool>(PyLong_AsLong(pValue));

}

CPythonObjectNew PythonObjectBuilder::setInt32(const int32_t value)
{
	CPythonObjectNew returnVal(PyLong_FromLong(static_cast<long>(value)));
	return returnVal;
}

CPythonObjectNew PythonObjectBuilder::setDouble(const double value)
{
	CPythonObjectNew returnVal(PyFloat_FromDouble(value));
	return returnVal;
}

CPythonObjectNew PythonObjectBuilder::setString(const std::string& value)
{
	CPythonObjectNew returnVal(PyUnicode_FromString(value.c_str()));
	return returnVal;
}

CPythonObjectNew PythonObjectBuilder::setBool(const bool value)
{
	CPythonObjectNew returnVal(PyBool_FromLong(value));
	return returnVal;
}
