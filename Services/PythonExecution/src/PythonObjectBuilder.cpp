#include "PythonObjectBuilder.h"
#include "PythonException.h"


void PythonObjectBuilder::StartTupleAssemply(int size)
{
	if (size != 0)
	{
		_assembly = (PyTuple_New(size));
	}
	_assemblySize = size;
	_currentSize = 0;
}

void PythonObjectBuilder::operator<<(CPythonObject* newEntry)
{
	if (_currentSize < _assemblySize)
	{
		int success = PyTuple_SetItem(_assembly,_currentSize,*newEntry);
		assert(success == 0);
		newEntry->DropOwnership();
		_currentSize++;
	}
	else
	{
		throw std::exception("Exceeding boundary of tuple.");
	}
}

CPythonObjectNew PythonObjectBuilder::getAssembledTuple()
{
	CPythonObjectNew returnValue(_assembly);
	_assembly = nullptr;
	return returnValue;
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

CPythonObjectBorrowed PythonObjectBuilder::getTupleItem(const CPythonObject& pValue, int position, const std::string& varName)
{
	CPythonObjectBorrowed tupleEntity(PyTuple_GetItem(pValue, position));
	if (tupleEntity == nullptr)
	{
		throw PythonException("For " + varName + " :");
	}
	return tupleEntity;
}

int32_t PythonObjectBuilder::getInt32ValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName)
{
	CPythonObjectBorrowed tupleEntity(PyTuple_GetItem(pValue, position));
	if (tupleEntity == nullptr)
	{
		throw PythonException();
	}
	return getInt32Value(tupleEntity, varName);
}

double PythonObjectBuilder::getDoubleValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName)
{
	CPythonObjectBorrowed tupleEntity( PyTuple_GetItem(pValue, position));
	if (tupleEntity == nullptr)
	{
		throw PythonException();
	}
	return getDoubleValue(tupleEntity, varName);
}

std::string PythonObjectBuilder::getStringValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName)
{
	CPythonObjectBorrowed tupleEntity (PyTuple_GetItem(pValue, position));
	if (tupleEntity == nullptr)
	{
		throw PythonException();
	}
	return getStringValue(tupleEntity, varName);
}

bool PythonObjectBuilder::getBoolValueFromTuple(const CPythonObject& pValue, int position, const std::string& varName)
{
	CPythonObjectBorrowed tupleEntity(PyTuple_GetItem(pValue, position));
	if (tupleEntity == nullptr)
	{
		throw PythonException();
	}
	return getBoolValue(tupleEntity, varName);
}

CPythonObjectBorrowed PythonObjectBuilder::getDictItem(const CPythonObject& pValue)
{
	if (!PyDict_Check(pValue))
	{
		throw std::exception("PythonObjectBuilder received invalid type. Expected type: PyDict.");
	}
	return CPythonObjectBorrowed(PyDict_Items(pValue));
}



CPythonObjectBorrowed PythonObjectBuilder::getListItem(const CPythonObject& pValue, int position)
{
	if (!PyList_Check(pValue))
	{
		throw std::exception("PythonObjectBuilder received invalid type. Expected type: PyList.");
	}
	return CPythonObjectBorrowed(PyList_GetItem(pValue, position));
}

std::list<int32_t> PythonObjectBuilder::getInt32List(const CPythonObject& pValue, const std::string& varName)
{
	if (PyList_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}
	
	std::list<int32_t> values;
	for (auto i = 0; i < PyList_Size(pValue); i++)
	{
		CPythonObjectBorrowed listItem(PyList_GetItem(pValue, i));
		values.push_back(getInt32Value(pValue,"ListItem"));
	}
	return values;
}

std::list<double> PythonObjectBuilder::getDoubleList(const CPythonObject& pValue, const std::string& varName)
{
	if (PyList_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}

	std::list<double> values;
	for (auto i = 0; i < PyList_Size(pValue); i++)
	{
		CPythonObjectBorrowed listItem(PyList_GetItem(pValue, i));
		values.push_back(getDoubleValue(pValue, "ListItem"));
	}
	return values;
}

std::list<std::string> PythonObjectBuilder::getStringList(const CPythonObject& pValue, const std::string& varName)
{
	if (PyList_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}

	std::list<std::string> values;
	auto listSize = PyList_Size(pValue);
	for (auto i = 0; i < listSize; i++)
	{
		CPythonObjectBorrowed listItem(PyList_GetItem(pValue, i));
		values.push_back(getStringValue(listItem, "ListItem"));
	}

	return values;
}

std::list<bool> PythonObjectBuilder::getBoolList(const CPythonObject& pValue, const std::string& varName)
{
	if (PyList_Check(pValue) != 1)
	{
		throw std::exception(("The variable " + varName + " is not of the requested type.").c_str());
	}

	std::list<bool> values;
	for (auto i = 0; i < PyList_Size(pValue); i++)
	{
		CPythonObjectBorrowed listItem(PyList_GetItem(pValue, i));
		values.push_back(getBoolValue(pValue, "ListItem"));
	}
	return values;
}

std::optional<ot::Variable> PythonObjectBuilder::getVariable(CPythonObject& pValue)
{
	if (PyFloat_Check(pValue))
	{
		return ot::Variable(static_cast<double>(PyFloat_AsDouble(pValue)));
	}
	else if (PyBool_Check(pValue))
	{
		return ot::Variable(static_cast<bool>(PyLong_AsLong(pValue)));
	}
	else if (PyLong_Check(pValue))
	{
		return ot::Variable(static_cast<int64_t>(PyLong_AsLongLong(pValue)));
	}
	else if (PyUnicode_Check(pValue))
	{
		return ot::Variable(PyUnicode_AsUTF8(pValue));
	}
	else if (Py_None == pValue)
	{
		return {};
	}
	else
	{
		throw std::exception("Not supported type by PythonObjectBuilder.");
	}
}

CPythonObjectNew PythonObjectBuilder::setInt64(const int64_t value)
{
	CPythonObjectNew returnVal(PyLong_FromLongLong(static_cast<long long>(value)));
	return returnVal;
}

CPythonObjectNew PythonObjectBuilder::setInt32(const int32_t value)
{
	CPythonObjectNew returnVal(PyLong_FromLong(static_cast<long>(value)));
	return returnVal;
}

CPythonObjectNew PythonObjectBuilder::setDouble(double value)
{
	PyObject* pvalue = PyFloat_FromDouble(value);

	CPythonObjectNew returnVal(pvalue);
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

CPythonObjectNew PythonObjectBuilder::setVariableList(std::list<ot::Variable>& values)
{
	uint64_t assemblySize =values.size();
	PyObject* assembly = PyTuple_New(assemblySize);
	uint64_t counter = 0;
	for (ot::Variable& value : values)
	{
		CPythonObjectNew pValue(nullptr);
		if (value.isInt32())
		{
			pValue.reset(setInt32(value.getInt32()));
		}
		else if (value.isInt64())
		{
			pValue.reset(setInt32(static_cast<int32_t>(value.getInt64())));
		}
		else if (value.isDouble())
		{
			pValue.reset(setDouble(value.getDouble()));
		}
		else if (value.isFloat())
		{
			pValue.reset(setDouble(static_cast<double>(value.getFloat())));
		}
		else if (value.isConstCharPtr())
		{
			pValue.reset(setString(std::string(value.getConstCharPtr())));
		}
		else if (value.isBool())
		{
			pValue.reset(setBool(value.getBool()));
		}
		else
		{
			throw std::exception("Type is not supported by the python service.");
		}

		int success = PyTuple_SetItem(assembly, counter, pValue);
		assert(success == 0);
		pValue.DropOwnership();
		counter++;
	}
	
	return CPythonObjectNew(assembly);
}

CPythonObjectNew PythonObjectBuilder::setVariableList(OT_rJSON_val& values)
{
	auto valueJSONArray = values.GetArray();
	uint64_t assemblySize = valueJSONArray.Size();
	PyObject* assembly = PyTuple_New(assemblySize);
	uint64_t counter = 0;

	for (auto& value : valueJSONArray)
	{
		CPythonObjectNew pValue(nullptr);
		if (value.IsInt())
		{
			pValue.reset(setInt32(value.GetInt()));
		}
		else if (value.IsInt64())
		{
			pValue.reset(setInt32(static_cast<int32_t>(value.GetInt64())));
		}
		else if (value.IsDouble())
		{
			pValue.reset(setDouble(value.GetDouble()));
		}
		else if (value.IsFloat())
		{
			pValue.reset(setDouble(static_cast<double>(value.GetFloat())));
		}
		else if (value.IsString())
		{
			pValue.reset(setString(std::string(value.GetString())));
		}
		else if (value.IsBool())
		{
			pValue.reset(setBool(value.GetBool()));
		}
		else
		{
			throw std::exception("Type is not supported by the python service.");
		}

		int success = PyTuple_SetItem(assembly, counter, pValue);
		assert(success == 0);
		pValue.DropOwnership();
		counter++;
	}

	return CPythonObjectNew(assembly);
}
