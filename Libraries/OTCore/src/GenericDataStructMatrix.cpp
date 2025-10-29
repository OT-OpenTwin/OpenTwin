// @otlicense

#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"

using namespace ot;

ot::GenericDataStructMatrix::GenericDataStructMatrix(uint32_t _rows, uint32_t _columns) 
	: GenericDataStruct(getClassName(), _rows * _columns), m_numberOfColumns(_columns), m_numberOfRows(_rows)
{
	allocateValueMemory();
}

GenericDataStructMatrix::GenericDataStructMatrix(const MatrixEntryPointer& _matrixEntryPointer)
	: GenericDataStruct(getClassName(), _matrixEntryPointer.m_row * _matrixEntryPointer.m_column), m_numberOfColumns(_matrixEntryPointer.m_column), m_numberOfRows(_matrixEntryPointer.m_row)
{
	allocateValueMemory();
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(uint32_t _rows, uint32_t _columns, ot::Variable _defaultValue)
	: GenericDataStruct(getClassName(), _rows* _columns), m_numberOfColumns(_columns), m_numberOfRows(_rows) 
{
	allocateValueMemory(_defaultValue);
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(const MatrixEntryPointer& _matrixEntryPointer, ot::Variable _defaultValue)
	: GenericDataStruct(getClassName(), _matrixEntryPointer.m_row * _matrixEntryPointer.m_column), m_numberOfColumns(_matrixEntryPointer.m_column), m_numberOfRows(_matrixEntryPointer.m_row)
{
	allocateValueMemory(_defaultValue);
}

GenericDataStructMatrix::GenericDataStructMatrix()
	: GenericDataStruct(getClassName())
{}

GenericDataStructMatrix::~GenericDataStructMatrix()
{}

ot::GenericDataStructMatrix::GenericDataStructMatrix(const GenericDataStructMatrix& _other)
	: GenericDataStruct(getClassName(),_other.m_numberOfEntries),m_values(_other.m_values), m_numberOfColumns(_other.m_numberOfColumns), m_numberOfRows(_other.m_numberOfRows)
{
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(GenericDataStructMatrix&& _other)noexcept
	: GenericDataStruct(getClassName(), _other.m_numberOfEntries), m_values(std::move(_other.m_values)), m_numberOfColumns(std::move(_other.m_numberOfColumns)), m_numberOfRows(std::move(_other.m_numberOfRows))
{
	_other.m_numberOfEntries = 0;
}

void GenericDataStructMatrix::setValue(const MatrixEntryPointer& _matrixEntryPointer, ot::Variable&& _value)
{
	const uint32_t index = getIndex(_matrixEntryPointer.m_column, _matrixEntryPointer.m_row);
	m_values[index] = std::move(_value);
}

void GenericDataStructMatrix::setValue(const MatrixEntryPointer& _matrixEntryPointer, const ot::Variable& _value)
{
	const uint32_t index = getIndex(_matrixEntryPointer.m_column,_matrixEntryPointer.m_row);
	m_values[index] = _value;
}

void ot::GenericDataStructMatrix::setValues(const ot::Variable* _values, uint32_t _size)
{
	assert(_size == getNumberOfEntries());
	m_values.clear();
	m_values.reserve(_size);
	m_values.insert(m_values.begin(), &_values[0], &_values[_size]);
}

void ot::GenericDataStructMatrix::setValues(std::list<ot::Variable> _values)
{
	assert(_values.size() == getNumberOfEntries());
	m_values = { _values.begin(), _values.end() };
}

const ot::Variable& GenericDataStructMatrix::getValue(const MatrixEntryPointer& _matrixEntryPointer) const
{
	assert(_matrixEntryPointer.m_column < m_numberOfColumns);
	assert(_matrixEntryPointer.m_row < m_numberOfRows);
	const uint32_t index =  getIndex(_matrixEntryPointer.m_column,_matrixEntryPointer.m_row);
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