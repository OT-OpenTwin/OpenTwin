#include "PythonObjectBuilder.h"
#include "PythonException.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"
#include <iterator>

#define NO_IMPORT_ARRAY
#define PY_ARRAY_UNIQUE_SYMBOL PythonWrapper_ARRAY_API
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include "numpy/ndarrayobject.h"

void* PythonObjectBuilder::variableArrayToVoidArray(const ot::Variable* values, const uint32_t size, int& pType)
{
	const ot::Variable& firstVal = values[0];

	if (firstVal.isBool())
	{
		pType = NPY_BOOL;
		bool* data = new bool[size];
		for (uint32_t i = 0; i < size; i++)
		{
			data[i] = values[i].getBool();
		}
		return data;
	}
	else if (firstVal.isConstCharPtr())
	{
		//char* data = new char[size];
		//for (uint32_t i = 0; i < size; i++)
		//{
		//	data[i] = values[i].getConstCharPtr();
		//}
		//return data;
		return nullptr;
	}
	else if (firstVal.isDouble())
	{
		pType = NPY_DOUBLE;
		double* data = new double[size];
		for (uint32_t i = 0; i < size; i++)
		{
			data[i] = values[i].getDouble();
		}
		return data;
	}
	else if (firstVal.isFloat())
	{
		pType = NPY_FLOAT;
		float* data = new float[size];
		for (uint32_t i = 0; i < size; i++)
		{
			data[i] = values[i].getFloat();
		}
		return data;
	}
	else if (firstVal.isInt32())
	{
		pType = NPY_INT;
		int32_t* data = new int32_t[size];
		for (uint32_t i = 0; i < size; i++)
		{
			data[i] = values[i].getInt32();
		}
		return data;
	}
	else
	{
		assert(firstVal.isInt64());
		pType = NPY_LONG;
		int64_t* data = new int64_t[size];
		for (uint32_t i = 0; i < size; i++)
		{
			data[i] = values[i].getInt64();
		}
		return data;
	}
}

const ot::Variable* PythonObjectBuilder::voidArrayToVariableArray(void* data, const uint32_t size, int type)
{
	std::function<ot::Variable(void*, uint32_t)> cast;
	if (type == NPY_BOOL)
	{
		cast = [](void* data, uint32_t index) 
		{
			const bool castedData = ((bool*)data)[index];
			return ot::Variable(castedData); 
		};
	}
	else if (type == NPY_INT)
	{

		cast = [](void* data, uint32_t index) 
		{
			const int32_t castedData = ((int*)data)[index];
			return ot::Variable(castedData);
		};
	}
	else if (type == NPY_INT64)
	{
		cast = [](void* data, uint32_t index) 
		{
			const int64_t castedData = ((int64_t*)data)[index];
			return ot::Variable(castedData); 
		};
	}
	else if (type == NPY_DOUBLE)
	{
		cast = [](void* data, uint32_t index) 
		{
			const double castedData = ((double*)data)[index]; 
			return ot::Variable(castedData);
		};
	}
	else
	{
		assert(type == NPY_FLOAT);
		cast = [](void* data, uint32_t index) 
		{
			const float castedData = ((float*)data)[index];
			return ot::Variable(castedData);
		};
	}
	ot::Variable* variables = new ot::Variable[size];
	for (uint32_t index = 0; index < size; index++)
	{
		variables[index] = cast(data,index);
	}

	return variables;
}

PythonObjectBuilder::PythonObjectBuilder()
	: _assembly(nullptr)
{
}

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

ot::GenericDataStructList PythonObjectBuilder::getGenericDataStructList(CPythonObject& pValue)
{
	if (PyList_Check(pValue))
	{
		ot::GenericDataStructList data;
		for (auto i = 0; i < PyList_Size(pValue); i++)
		{
			CPythonObjectBorrowed listItem(PyList_GetItem(pValue, i));
			const auto dataEntry = getGenericDataStruct(listItem);
			data.push_back(dataEntry);
		}
		return data;
	}
	else if (PyTuple_Check(pValue))
	{
		ot::GenericDataStructList data;
		for (auto i = 0; i < PyTuple_Size(pValue); i++)
		{
			CPythonObjectBorrowed listItem(PyTuple_GetItem(pValue, i));
			const auto dataEntry = getGenericDataStruct(listItem);
			data.push_back(dataEntry);
		}
		return data;
	}
	else
	{
		return { getGenericDataStruct(pValue) };
	}
}

ot::GenericDataStruct* PythonObjectBuilder::getGenericDataStruct(CPythonObject& pValue)
{
	if (PyList_Check(pValue) || PyTuple_Check(pValue))
	{
		std::list<ot::Variable> varList = getVariableList(pValue);
		ot::GenericDataStructVector* entry(new ot::GenericDataStructVector());
		std::vector< ot::Variable> varVect(varList.begin(), varList.end());
		entry->setValues(std::move(varVect));
		return entry;
	}
	else if (PyArray_Check(pValue))
	{
		PyObject* pValueBase = pValue;
		npy_intp* shape = PyArray_SHAPE((PyArrayObject*)pValueBase);
		int type = PyArray_TYPE((PyArrayObject*)pValueBase);
		int dimensions = PyArray_NDIM((PyArrayObject*)pValueBase);

		ot::MatrixEntryPointer matrixEntry;
		matrixEntry.m_row= static_cast<uint32_t>(shape[0]);
		if (dimensions > 1)
		{
			matrixEntry.m_column = static_cast<uint32_t>(shape[1]);
		}
		else
		{
			matrixEntry.m_column = 1;
		}
		uint32_t size = matrixEntry.m_row * matrixEntry.m_column;
		ot::GenericDataStructMatrix* matrix(new ot::GenericDataStructMatrix(matrixEntry));
		void* pyArrayData =	PyArray_DATA((PyArrayObject*)pValueBase);
		const ot::Variable* variableArray = voidArrayToVariableArray(pyArrayData, size, type);
		matrix->setValues(variableArray, size);
		return matrix;
	}
	else
	{
		ot::GenericDataStructSingle* entry(new ot::GenericDataStructSingle());
		std::optional<ot::Variable> var = getVariable(pValue);
		if (var.has_value())
		{
			entry->setValue(var.value());
			return entry;
		}
		else
		{
			return nullptr;
		}
	}
}

std::list<ot::Variable> PythonObjectBuilder::getVariableList(CPythonObject& pValue)
{
	if (PyList_Check(pValue))
	{
		std::list<ot::Variable> variables{};
		for (auto i = 0; i < PyList_Size(pValue); i++)
		{
			CPythonObjectBorrowed listItem(PyList_GetItem(pValue, i));

			std::optional<ot::Variable> variable = getVariable(listItem);
			if (variable.has_value())
			{
				variables.push_back(variable.value());
			}
		}
		return variables;
	}
	else if (PyTuple_Check(pValue))
	{
		std::list<ot::Variable> variables{};
		for (auto i = 0; i < PyTuple_Size(pValue); i++)
		{
			CPythonObjectBorrowed listItem(PyTuple_GetItem(pValue, i));

			std::optional<ot::Variable> variable = getVariable(listItem);
			if (variable.has_value())
			{
				variables.push_back(variable.value());
			}
		}
		return variables;
	}
	else
	{ 
		std::optional<ot::Variable> variable = getVariable(pValue);
		if (variable.has_value())
		{
			return { variable.value() };
		}
		else
		{
			return{};
		}
	}
}

std::optional<ot::Variable> PythonObjectBuilder::getVariable(CPythonObject& pValue)
{
	if (PyFloat_Check(pValue))
	{
		return { ot::Variable(static_cast<double>(PyFloat_AsDouble(pValue))) };
	}
	else if (PyBool_Check(pValue))
	{
		return { ot::Variable(static_cast<bool>(PyLong_AsLong(pValue))) };
	}
	else if (PyLong_Check(pValue))
	{
		return { ot::Variable(static_cast<int64_t>(PyLong_AsLongLong(pValue))) };
	}
	else if (PyUnicode_Check(pValue))
	{
		return { ot::Variable(PyUnicode_AsUTF8(pValue)) };
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



CPythonObjectNew PythonObjectBuilder::setVariableTuple(const std::list<ot::Variable>& values)
{
	uint64_t assemblySize = values.size();
	PyObject* assembly = PyTuple_New(assemblySize);
	uint64_t counter = 0;
	for (const ot::Variable& value : values)
	{

		CPythonObjectNew pValue = setVariable(value);
		int success = PyTuple_SetItem(assembly, counter, pValue);
		assert(success == 0);
		pValue.DropOwnership();
		counter++;
	}
	return CPythonObjectNew(assembly);
}

CPythonObjectNew PythonObjectBuilder::setVariableList(const std::list<ot::Variable>& values)
{
	uint64_t assemblySize =values.size();
	PyObject* assembly = PyList_New(assemblySize);
	uint64_t counter = 0;
	for (const ot::Variable& value : values)
	{
		
		CPythonObjectNew pValue = setVariable(value);
		int success = PyList_SetItem(assembly, counter, pValue);
		assert(success == 0);
		pValue.DropOwnership();
		counter++;
	}
	return CPythonObjectNew(assembly);
}

CPythonObjectNew PythonObjectBuilder::setVariableList(const std::vector<ot::Variable>& values)
{
	uint64_t assemblySize = values.size();
	PyObject* assembly = PyList_New(assemblySize);
	uint64_t counter = 0;
	for (const ot::Variable& value : values)
	{
		CPythonObjectNew pValue = setVariable(value);
		int success = PyList_SetItem(assembly, counter, pValue);
		assert(success == 0);
		pValue.DropOwnership();
		counter++;
	}

	return CPythonObjectNew(assembly);
}

CPythonObjectNew PythonObjectBuilder::setVariable(const ot::Variable& value)
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
	return pValue;
}

CPythonObjectNew PythonObjectBuilder::setVariableList(rapidjson::GenericArray<false, rapidjson::Value>& valueJSONArray)
{
	uint64_t assemblySize = valueJSONArray.Size();
	//PyObject* assembly = PyTuple_New(assemblySize);
	PyObject* assembly = PyList_New(assemblySize);
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

		int success = PyList_SetItem(assembly, counter, pValue);
		assert(success == 0);
		pValue.DropOwnership();
		counter++;
	}

	return CPythonObjectNew(assembly);
}


CPythonObjectNew PythonObjectBuilder::setGenericDataStruct(ot::GenericDataStruct* genericDataStruct)
{
	auto singleVal = dynamic_cast<ot::GenericDataStructSingle*>(genericDataStruct);
	if (singleVal != nullptr)
	{
		return setVariable(singleVal->getValue());
	}

	auto vectVal = dynamic_cast<ot::GenericDataStructVector*>(genericDataStruct);
	if (vectVal != nullptr)
	{
		return setVariableList(vectVal->getValues());
	}

	auto matrixVal = dynamic_cast<ot::GenericDataStructMatrix*>(genericDataStruct);
	assert(matrixVal != nullptr);

	uint32_t rows = matrixVal->getNumberOfRows();
	uint32_t columns = matrixVal->getNumberOfColumns();
	int type;
	npy_intp* pColumns = new npy_intp[rows];
	for (uint32_t i = 0; i < rows; i++)
	{
		pColumns[i] = columns;
	}

	const ot::Variable* values =	matrixVal->getValues();
	const uint32_t numberOfEntries =	matrixVal->getNumberOfEntries();
	void* data = variableArrayToVoidArray(values, numberOfEntries, type);
			
	PyObject* pMatrix = PyArray_SimpleNewFromData(rows, pColumns, type, data);
			
	return CPythonObjectNew(pMatrix);
}

CPythonObjectNew PythonObjectBuilder::setGenericDataStructList(ot::GenericDataStructList& values)
{
	uint64_t assemblySize = values.size();
	PyObject* assembly = PyList_New(assemblySize);
	uint64_t counter = 0;
	
	if (values.size() == 1)
	{
		auto value=	values.begin();
		auto vectorValue = dynamic_cast<ot::GenericDataStructVector*>(*value);
		if (vectorValue != nullptr)
		{
			CPythonObjectNew pValue = setGenericDataStruct(*value);
			return pValue;
		}
	}
	
	for (ot::GenericDataStruct* value : values)
	{
		CPythonObjectNew pValue = setGenericDataStruct(value);
		int success = PyList_SetItem(assembly, counter, pValue);
		assert(success == 0);
		pValue.DropOwnership();
		counter++;
	}

	return CPythonObjectNew(assembly);
}
