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
{

}

GenericDataStructMatrix::~GenericDataStructMatrix()
{
	//free(_values);
	//_values = NULL;
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(const GenericDataStructMatrix& other)
	:GenericDataStruct(getClassName(),other._numberOfEntries),_values(other._values)
{
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(GenericDataStructMatrix&& other)
	:GenericDataStruct(getClassName(), other._numberOfEntries), _values(std::move(other._values))
{
	other._numberOfEntries = 0;
}

GenericDataStructMatrix& ot::GenericDataStructMatrix::operator=(const GenericDataStructMatrix& other)
{
	_values = other._values;
	_numberOfEntries = other._numberOfEntries;
	return *this;
}

GenericDataStructMatrix& ot::GenericDataStructMatrix::operator=(GenericDataStructMatrix&& other)
{
	_values = std::move(other._values);
	_numberOfEntries = other._numberOfEntries;
	other._numberOfEntries = 0;
	return *this;
}

void GenericDataStructMatrix::setValue(uint32_t columnIndex, uint32_t rowIndex, ot::Variable&& value)
{
	/*const uint32_t index = getIndex(columnIndex, rowIndex);
	_values[index] = value;*/
	_values[rowIndex][columnIndex] = std::move(value);
}

void GenericDataStructMatrix::setValue(uint32_t columnIndex, uint32_t rowIndex, const ot::Variable& value)
{
	_values[rowIndex][columnIndex] = value;
}

const ot::Variable& GenericDataStructMatrix::getValue(uint32_t columnIndex, uint32_t rowIndex) const
{
	/*const uint32_t index =  getIndex(columnIndex, rowIndex);
	return _values[index];*/
	return _values[rowIndex][columnIndex];
}

void ot::GenericDataStructMatrix::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	GenericDataStruct::addToJsonObject(_object, _allocator);
	_object.AddMember("numberOfColumns", _numberOfColumns, _allocator);
	_object.AddMember("numberOfRows", _numberOfRows, _allocator);

	VariableToJSONConverter converter;
	ot::JsonArray outerArr;
	for (auto& row : _values)
	{
		ot::JsonArray innerArr;
		for (auto& columnVal : row)
		{
			innerArr.PushBack(converter(columnVal,_allocator),_allocator);
		}
		outerArr.PushBack(innerArr, _allocator);
	}
	_object.AddMember("values", outerArr, _allocator);
}

void ot::GenericDataStructMatrix::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	GenericDataStruct::setFromJsonObject(_object);
	_numberOfColumns = ot::json::getUInt(_object, "numberOfColumns");
	_numberOfRows = ot::json::getUInt(_object, "numberOfRows");
	_numberOfEntries = _numberOfColumns * _numberOfRows;
	AllocateValueMemory();

	JSONToVariableConverter converter;
	auto outerArray = ot::json::getArray(_object, "values");
	for (uint32_t i = 0; i < outerArray.Size(); i++)
	{
		auto innerArray = ot::json::getArray(outerArray, i);
		for (uint32_t j = 0; j < innerArray.Size(); j++)
		{
			_values[i][j] = converter(innerArray[j]);
		}
	}
}

void ot::GenericDataStructMatrix::AllocateValueMemory()
{
	_values.resize(_numberOfRows);
	for (uint32_t row = 0; row < _numberOfRows; row++)
	{
		_values[row].resize(_numberOfColumns);
	}
	/*_values = static_cast<ot::Variable*>(malloc(sizeof(ot::Variable)*_numberOfEntries));*/
}
