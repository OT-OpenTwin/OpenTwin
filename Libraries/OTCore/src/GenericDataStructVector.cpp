#include "OTCore/GenericDataStructVector.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"

ot::GenericDataStructVector::GenericDataStructVector(const std::vector<ot::Variable>& values)
	:GenericDataStruct(getClassName(),static_cast<int32_t>(values.size()))
{
	_values = values;
}

ot::GenericDataStructVector::GenericDataStructVector(std::vector<ot::Variable>&& values)
	:GenericDataStruct(getClassName(), static_cast<int32_t>(values.size()))
{
	_values = std::move(values);
}

ot::GenericDataStructVector::GenericDataStructVector(uint32_t numberOfEntries)
	:GenericDataStruct(getClassName(), numberOfEntries)
{
	AllocateValueMemory();
}

ot::GenericDataStructVector& ot::GenericDataStructVector::operator=(const GenericDataStructVector& other)
{
	_values = other._values;
	_numberOfEntries = other._numberOfEntries;
	return *this;
}

ot::GenericDataStructVector& ot::GenericDataStructVector::operator=(GenericDataStructVector&& other)
{
	_values = std::move(other._values);
	_numberOfEntries = other._numberOfEntries;
	other._numberOfEntries = 0;
	return *this;
}

ot::GenericDataStructVector::GenericDataStructVector()
	:GenericDataStruct(getClassName())
{
}

ot::GenericDataStructVector::GenericDataStructVector(const GenericDataStructVector& other)
	:GenericDataStruct(getClassName(),other._numberOfEntries), _values(other._values)
{
}

ot::GenericDataStructVector::GenericDataStructVector(GenericDataStructVector&& other)
	:GenericDataStruct(getClassName(), other._numberOfEntries), _values(std::move(other._values))
{
	other._numberOfEntries = 0;
}

void ot::GenericDataStructVector::setValue(uint32_t index, const ot::Variable& value)
{
	 _values[index] = value; 
	 _numberOfEntries = static_cast<uint32_t>(_values.size()); 
}

void ot::GenericDataStructVector::setValue(uint32_t index, ot::Variable&& value)
{
	_values[index] = std::move(value); 
	_numberOfEntries = static_cast<uint32_t>(_values.size());
}

void ot::GenericDataStructVector::setValues(const std::vector<ot::Variable>& values)
{
	_values = values;
	_numberOfEntries = static_cast<uint32_t>(_values.size());
}

void ot::GenericDataStructVector::setValues(std::vector<ot::Variable>&& values)
{
	_values = std::move(values);
	_numberOfEntries = static_cast<uint32_t>(_values.size());
}

void ot::GenericDataStructVector::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	VariableToJSONConverter converter;
	GenericDataStruct::addToJsonObject(_object, _allocator);
	ot::JsonArray arr;
	for (auto& value: _values)
	{
		arr.PushBack(converter(value, _allocator), _allocator);
	}
	_object.AddMember("values", arr, _allocator);
}

void ot::GenericDataStructVector::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	GenericDataStruct::setFromJsonObject(_object);
	AllocateValueMemory();
	JSONToVariableConverter converter;
	auto array = ot::json::getArray(_object, "values");;
	for (uint32_t j = 0; j < array.Size(); j++)
	{
		_values[j] = converter(array[j]);
	}
}

void ot::GenericDataStructVector::AllocateValueMemory()
{
	_values.resize(_numberOfEntries);
}
