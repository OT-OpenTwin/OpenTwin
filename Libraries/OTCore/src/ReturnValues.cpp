#include "OTCore/ReturnValues.h"

#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/GenericDataStructFactory.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"

ot::ReturnValues::~ReturnValues()
{

}

ot::ReturnValues::ReturnValues(const ReturnValues& other)
{
	*this = other;
}

ot::ReturnValues& ot::ReturnValues::operator=(const ReturnValues& _other)
{
	if (*this != _other)
	{
		m_valuesByName = _other.m_valuesByName;
	}
	return *this;
}


bool ot::ReturnValues::operator==(const ReturnValues& other) const
{
	return m_valuesByName == other.m_valuesByName;
}

bool ot::ReturnValues::operator!=(const ReturnValues& other) const
{
	return !(*this == other);
}

void ot::ReturnValues::addData(const std::string& _entryName, const ot::JsonDocument* _values)
{
	m_valuesByName[_entryName] = _values;
}

void ot::ReturnValues::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	ot::JsonObject fullEntry;
	for (auto& element : m_valuesByName)
	{
		const std::string name = element.first;
		ot::JsonString key(name,_allocator);
		
		const ot::JsonDocument* values = element.second;
		ot::JsonValue data;
		data.CopyFrom(*values, _allocator);

		fullEntry.AddMember(key, data, _allocator);
	}

	_object.AddMember(ot::JsonString("ReturnValues", _allocator), fullEntry, _allocator);
}

void ot::ReturnValues::setFromJsonObject(const ot::ConstJsonObject& _object)
{	
	auto& returnValues = _object["ReturnValues"];
	m_valuesAsDoc.CopyFrom(returnValues, m_valuesAsDoc.GetAllocator());
}
