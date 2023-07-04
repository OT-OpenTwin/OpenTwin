#include "PythonObjectBuilder.h"

void PythonObjectBuilder::StartTupleAssemply(int size)
{
	if (size != 0)
	{
		_assembly.reset(PyTuple_New(size));
	}
	_assemblySize = size;
	_currentSize = 0;
}

void PythonObjectBuilder::operator<<(CPythonObject&& newEntry)
{
	if (_currentSize < _assemblySize)
	{
		int success = PyTuple_SetItem(_assembly,_currentSize,newEntry);
		assert(success == 0);
		newEntry.DropOwnership();
		_currentSize++;
	}
	else
	{
		throw std::exception("Exceeding boundary of tuple.");
	}
}

CPythonObjectNew PythonObjectBuilder::getAssembledTuple()
{
	return std::move(_assembly);
}

int32_t PythonObjectBuilder::getInt32Value(const CPythonObject& pValue, const std::string& varName)
{
	if (PyLong_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return static_cast<int32_t>(PyLong_AsLong(pValue));
}

double PythonObjectBuilder::getDoubleValue(const CPythonObject& pValue, const std::string& varName)
{
	if (PyFloat_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return PyFloat_AsDouble(pValue);
}

std::string PythonObjectBuilder::getStringValue(const CPythonObject& pValue, const std::string& varName)
{
	if (PyUnicode_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return std::string(PyUnicode_AsUTF8(pValue));
}

bool PythonObjectBuilder::getBoolValue(const CPythonObject& pValue, const std::string& varName)
{
	if (PyBool_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	return static_cast<bool>(PyLong_AsLong(pValue));

}

int32_t PythonObjectBuilder::getInt32ValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName)
{

	return PythonObjectBuilder::INSTANCE()->getInt32Value(PyTuple_GetItem(pValue, 0), varName);
}

double PythonObjectBuilder::getDoubleValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName)
{
	return 0.0;
}

std::string PythonObjectBuilder::getStringValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName)
{
	return std::string();
}

bool PythonObjectBuilder::getBoolValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName)
{
	return false;
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
