#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"

using namespace ot;

GenericDataStructMatrix::GenericDataStructMatrix(uint32_t _numberOfColumns, uint32_t _numberOfRows)
	:GenericDataStruct(getClassName(), _numberOfColumns* _numberOfRows), m_numberOfColumns(_numberOfColumns), m_numberOfRows(_numberOfRows)
{
	allocateValueMemory();
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(uint32_t _numberOfColumns, uint32_t _numberOfRows, ot::Variable _defaultValue)
	:GenericDataStruct(getClassName(), _numberOfColumns* _numberOfRows), m_numberOfColumns(_numberOfColumns), m_numberOfRows(_numberOfRows)
{
	allocateValueMemory(_defaultValue);
}

GenericDataStructMatrix::GenericDataStructMatrix()
	:GenericDataStruct(getClassName())
{}

GenericDataStructMatrix::~GenericDataStructMatrix()
{}

ot::GenericDataStructMatrix::GenericDataStructMatrix(const GenericDataStructMatrix& _other)
	:GenericDataStruct(getClassName(),_other.m_numberOfEntries),m_values(_other.m_values)
{
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(GenericDataStructMatrix&& _other)noexcept
	:GenericDataStruct(getClassName(), _other.m_numberOfEntries), m_values(std::move(_other.m_values))
{
	_other.m_numberOfEntries = 0;
}

GenericDataStructMatrix& ot::GenericDataStructMatrix::operator=(const GenericDataStructMatrix& _other)
{
	m_values = _other.m_values;
	m_numberOfEntries = _other.m_numberOfEntries;
	return *this;
}

GenericDataStructMatrix& ot::GenericDataStructMatrix::operator=(GenericDataStructMatrix&& _other)noexcept
{
	m_values = std::move(_other.m_values);
	m_numberOfEntries = _other.m_numberOfEntries;
	_other.m_numberOfEntries = 0;
	return *this;
}

void GenericDataStructMatrix::setValue(uint32_t _columnIndex, uint32_t _rowIndex, ot::Variable&& _value)
{

	const uint32_t index = getIndex(_columnIndex, _rowIndex);
	m_values[index] = std::move(_value);
}

void GenericDataStructMatrix::setValue(uint32_t _columnIndex, uint32_t _rowIndex, const ot::Variable& _value)
{
	const uint32_t index = getIndex(_columnIndex, _rowIndex);
	m_values[index] = _value;
}

void ot::GenericDataStructMatrix::setValues(const ot::Variable* _values, uint32_t _size)
{
	m_values.clear();
	m_values.reserve(_size);
	m_values.insert(m_values.begin(), &_values[0], &_values[_size]);
}

const ot::Variable& GenericDataStructMatrix::getValue(uint32_t _columnIndex, uint32_t _rowIndex) const
{
	assert(_columnIndex < m_numberOfColumns);
	assert(_rowIndex < m_numberOfRows);
	const uint32_t index =  getIndex(_columnIndex, _rowIndex);
	return m_values[index];
}

const ot::Variable* ot::GenericDataStructMatrix::getValues() const
{
	return &m_values.front();
}

void ot::GenericDataStructMatrix::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	GenericDataStruct::addToJsonObject(_object, _allocator);
	_object.AddMember("numberOfColumns", m_numberOfColumns, _allocator);
	_object.AddMember("numberOfRows", m_numberOfRows, _allocator);

	VariableToJSONConverter converter;
	ot::JsonArray jArr;

	for (uint32_t index = 0;index < m_numberOfEntries; index++)
	{
		const ot::Variable& value = m_values[index];
		jArr.PushBack(converter(value,_allocator),_allocator);
	}
	_object.AddMember("values", jArr, _allocator);
}

void ot::GenericDataStructMatrix::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	GenericDataStruct::setFromJsonObject(_object);
	m_numberOfColumns = ot::json::getUInt(_object, "numberOfColumns");
	m_numberOfRows = ot::json::getUInt(_object, "numberOfRows");
	m_numberOfEntries = m_numberOfColumns * m_numberOfRows;
	
	JSONToVariableConverter converter;
	auto jArray = ot::json::getArray(_object, "values");
	allocateValueMemory();

	for (uint32_t index = 0; index < m_numberOfEntries; index++)
	{
		const ot::Variable& var = converter(jArray[index]);
		m_values[index] = var;
	}
}

void ot::GenericDataStructMatrix::allocateValueMemory()
{
	m_values.resize(m_numberOfEntries);
}

void ot::GenericDataStructMatrix::allocateValueMemory(const ot::Variable& _defaultValue)
{
	m_values.resize(m_numberOfEntries,_defaultValue);
}

void ot::GenericDataStructMatrix::allocateValueMemory(ot::Variable&& _defaultValue)
{
	m_values.resize(m_numberOfEntries, std::move(_defaultValue));
}