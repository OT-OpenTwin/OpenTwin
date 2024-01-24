#include "OTCore/GenericDataStructVector.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"

ot::GenericDataStructVector::GenericDataStructVector(uint32_t numberOfEntries)
	:GenericDataStruct(getClassName(), numberOfEntries)
{
	AllocateValueMemory();
}

ot::GenericDataStructVector::GenericDataStructVector()
	:GenericDataStruct(getClassName())
{
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
