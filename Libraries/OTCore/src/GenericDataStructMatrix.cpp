#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"

using namespace ot;

GenericDataStructMatrix::GenericDataStructMatrix(uint32_t numberOfColumns, uint32_t numberOfRows)
	:GenericDataStruct(getClassName(), numberOfColumns* numberOfRows), _numberOfColumns(numberOfColumns), _numberOfRows(numberOfRows)
{
	AllocateValueMemory();
}

GenericDataStructMatrix::GenericDataStructMatrix()
	:GenericDataStruct(getClassName())
{}

GenericDataStructMatrix::~GenericDataStructMatrix()
{
	/* delete[] _values;
	_values = nullptr;*/
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(const GenericDataStructMatrix& other)
	:GenericDataStruct(getClassName(),other._numberOfEntries),_values(other._values)//, _typeName(other._typeName)
{
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(GenericDataStructMatrix&& other)noexcept
	:GenericDataStruct(getClassName(), other._numberOfEntries), _values(std::move(other._values))//, _typeName(std::move(other._typeName))
{
	other._numberOfEntries = 0;
}

GenericDataStructMatrix& ot::GenericDataStructMatrix::operator=(const GenericDataStructMatrix& other)
{
	_values = other._values;
	_numberOfEntries = other._numberOfEntries;
	//_typeName = other._typeName;
	return *this;
}

GenericDataStructMatrix& ot::GenericDataStructMatrix::operator=(GenericDataStructMatrix&& other)noexcept
{
	_values = std::move(other._values);
	_numberOfEntries = other._numberOfEntries;
	//_typeName = other._typeName;
	other._numberOfEntries = 0;
	return *this;
}

void GenericDataStructMatrix::setValue(uint32_t columnIndex, uint32_t rowIndex, ot::Variable&& value)
{

	const uint32_t index = getIndex(columnIndex, rowIndex);
	_values[index] = std::move(value);
	//if (_values == nullptr)
	//{
	//	AllocateValueMemory(value.getTypeName());
	//}
	//_setterMove(index, std::move(value));
}

void GenericDataStructMatrix::setValue(uint32_t columnIndex, uint32_t rowIndex, const ot::Variable& value)
{
	const uint32_t index = getIndex(columnIndex, rowIndex);
	_values[index] = value;
	/*if (_values == nullptr)
	{
		AllocateValueMemory(value.getTypeName());
	}
	_setter(index, value);*/
}

void ot::GenericDataStructMatrix::setValues(const ot::Variable* values, uint32_t size)
{
	_values.clear();
	_values.reserve(size);
	_values.insert(_values.begin(), &values[0], &values[size]);
}

const ot::Variable& GenericDataStructMatrix::getValue(uint32_t columnIndex, uint32_t rowIndex) const
{
	const uint32_t index =  getIndex(columnIndex, rowIndex);
	return _values[index];
	/*return _getter(index);*/
}

const ot::Variable* ot::GenericDataStructMatrix::getValues() const
{
	return &_values.front();
}

void ot::GenericDataStructMatrix::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	GenericDataStruct::addToJsonObject(_object, _allocator);
	_object.AddMember("numberOfColumns", _numberOfColumns, _allocator);
	_object.AddMember("numberOfRows", _numberOfRows, _allocator);

	VariableToJSONConverter converter;
	ot::JsonArray jArr;

	for (uint32_t index = 0;index < _numberOfEntries; index++)
	{
		//const ot::Variable& value = _getter(index);
		const ot::Variable& value = _values[index];
		jArr.PushBack(converter(value,_allocator),_allocator);
	}
	_object.AddMember("values", jArr, _allocator);
}

void ot::GenericDataStructMatrix::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	GenericDataStruct::setFromJsonObject(_object);
	_numberOfColumns = ot::json::getUInt(_object, "numberOfColumns");
	_numberOfRows = ot::json::getUInt(_object, "numberOfRows");
	_numberOfEntries = _numberOfColumns * _numberOfRows;
	
	JSONToVariableConverter converter;
	auto jArray = ot::json::getArray(_object, "values");
	AllocateValueMemory(/*var.getTypeName()*/);
	//_setter(0, std::move(var));
	for (uint32_t index = 0; index < _numberOfEntries; index++)
	{
		const ot::Variable& var = converter(jArray[index]);
		_values[index] = var;
		//_setter(index, std::move(var));
	}
}

void ot::GenericDataStructMatrix::AllocateValueMemory(/*const std::string& typeName*/)
{
	_values.resize(_numberOfEntries);
	/*if (typeName == ot::TypeNames::getBoolTypeName())
	{
		_values = new bool[_numberOfEntries];
		_setter = [this](int32_t index,const ot::Variable& var) 
		{
			bool* temp = (bool*)_values;
			temp[index] = var.getBool(); 
		};
		_setterMove = [this](int32_t index, ot::Variable&& var)
		{
			bool* temp = (bool*)_values;
			temp[index] = std::move(var.getBool());
		};
		_getter = [this](int32_t index)
		{
			bool* temp = (bool*)_values;
			return ot::Variable(temp[index]);
		};
	}
	else if (typeName == ot::TypeNames::getCharTypeName())
	{
		_values = new char[_numberOfEntries];
		_setter = [this](int32_t index, const ot::Variable& var)
		{
			char* temp = (char*)_values;
			temp[index] = *const_cast<char*>(var.getConstCharPtr());
		};
		_setterMove = [this](int32_t index, ot::Variable&& var)
		{
			char* temp = (char*)_values;
			temp[index] = std::move(*const_cast<char*>(var.getConstCharPtr()));
		};
		_getter = [this](int32_t index)
		{
			char* temp = (char*)_values;
			return ot::Variable(temp[index]);
		};
	}
	else if (typeName == ot::TypeNames::getDoubleTypeName())
	{
		_values = new double[_numberOfEntries];
		_setter = [this](int32_t index, const ot::Variable& var)
		{
			double* temp = (double*)_values;
			temp[index] = var.getDouble();
		};
		_setterMove = [this](int32_t index, ot::Variable&& var)
		{
			double* temp = (double*)_values;
			temp[index] = std::move(var.getDouble());
		};
		_getter = [this](int32_t index)
		{
			double* temp = (double*)_values;
			return ot::Variable(temp[index]);
		};
	}
	else if (typeName == ot::TypeNames::getFloatTypeName())
	{
		_values = new float[_numberOfEntries];
		_setter = [this](int32_t index, const ot::Variable& var)
		{
			float* temp = (float*)_values;
			temp[index] = var.getFloat();
		};
		_setterMove = [this](int32_t index, ot::Variable&& var)
		{
			float* temp = (float*)_values;
			temp[index] = std::move(var.getFloat());
		};
		_getter = [this](int32_t index)
		{
			float* temp = (float*)_values;
			return ot::Variable(temp[index]);
		};
	}
	else if (typeName == ot::TypeNames::getInt32TypeName())
	{
		_values = new int32_t[_numberOfEntries];
		_setter = [this](int32_t index, const ot::Variable& var)
		{
			int32_t* temp = (int32_t*)_values;
			temp[index] = var.getInt32();
		};
		_setterMove = [this](int32_t index, ot::Variable&& var)
		{
			int32_t* temp = (int32_t*)_values;
			temp[index] = std::move(var.getInt32());
		};
		_getter = [this](int32_t index)
		{
			int32_t* temp = (int32_t*)_values;
			return ot::Variable(temp[index]);
		};
	}
	else
	{
		assert(typeName == ot::TypeNames::getInt64TypeName());
		_values = new int64_t[_numberOfEntries];
		_setter = [this](int64_t index, const ot::Variable& var)
		{
			int64_t* temp = (int64_t*)_values;
			temp[index] = var.getInt64();
		};
		_setterMove = [this](int32_t index, ot::Variable&& var)
		{
			int64_t* temp = (int64_t*)_values;
			temp[index] = std::move(var.getInt64());
		};
		_getter = [this](int32_t index)
		{
			int64_t* temp = (int64_t*)_values;
			return ot::Variable(temp[index]);
		};
	}
	_typeName = typeName;*/
}
