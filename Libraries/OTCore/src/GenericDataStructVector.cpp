// @otlicense

#include "OTCore/GenericDataStructVector.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"

ot::GenericDataStructVector::GenericDataStructVector(const std::vector<ot::Variable>& _values)
	:GenericDataStruct(getClassName(),static_cast<int32_t>(_values.size()))
{
	m_values = _values;
}

ot::GenericDataStructVector::GenericDataStructVector(std::vector<ot::Variable>&& _values) noexcept
	:GenericDataStruct(getClassName(), static_cast<int32_t>(_values.size()))
{
	m_values = std::move(_values);
}

ot::GenericDataStructVector::GenericDataStructVector(uint32_t _numberOfEntries)
	:GenericDataStruct(getClassName(), _numberOfEntries)
{
	allocateValueMemory();
}


ot::GenericDataStructVector::GenericDataStructVector()
	:GenericDataStruct(getClassName())
{
}

ot::GenericDataStructVector::GenericDataStructVector(const GenericDataStructVector& _other)
	:GenericDataStruct(getClassName(),_other.m_numberOfEntries), m_values(_other.m_values)
{
}

ot::GenericDataStructVector::GenericDataStructVector(GenericDataStructVector&& _other) noexcept
	:GenericDataStruct(getClassName(), _other.m_numberOfEntries), m_values(std::move(_other.m_values))
{
	_other.m_numberOfEntries = 0;
}

void ot::GenericDataStructVector::setValue(uint32_t _index, const ot::Variable& _value)
{
	 m_values[_index] = _value; 
	 m_numberOfEntries = static_cast<uint32_t>(m_values.size()); 
}

void ot::GenericDataStructVector::setValue(uint32_t _index, ot::Variable&& _value)
{
	m_values[_index] = std::move(_value); 
	m_numberOfEntries = static_cast<uint32_t>(m_values.size());
}

void ot::GenericDataStructVector::setValues(const std::vector<ot::Variable>& _values)
{
	m_values = _values;
	m_numberOfEntries = static_cast<uint32_t>(m_values.size());
}

void ot::GenericDataStructVector::setValues(std::vector<ot::Variable>&& _values)
{
	m_values = std::move(_values);
	m_numberOfEntries = static_cast<uint32_t>(m_values.size());
}

void ot::GenericDataStructVector::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	VariableToJSONConverter converter;
	GenericDataStruct::addToJsonObject(_object, _allocator);
	ot::JsonArray arr;
	for (auto& value: m_values)
	{
		arr.PushBack(converter(value, _allocator), _allocator);
	}
	_object.AddMember("values", arr, _allocator);
}

void ot::GenericDataStructVector::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	GenericDataStruct::setFromJsonObject(_object);
	allocateValueMemory();
	JSONToVariableConverter converter;
	auto array = ot::json::getArray(_object, "values");;
	for (uint32_t j = 0; j < array.Size(); j++)
	{
		m_values[j] = converter(array[j]);
	}
}

void ot::GenericDataStructVector::allocateValueMemory()
{
	m_values.resize(m_numberOfEntries);
}
