#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/VariableToJSONConverter.h"

ot::GenericDataStructSingle::GenericDataStructSingle()
	:ot::GenericDataStruct(getClassName(), 1)
{
}

ot::GenericDataStructSingle::GenericDataStructSingle(const GenericDataStructSingle& _other)
	:GenericDataStruct(getClassName(), _other.m_numberOfEntries), m_value(_other.m_value)
{
}

ot::GenericDataStructSingle::GenericDataStructSingle(GenericDataStructSingle&& _other) noexcept
	:GenericDataStruct(getClassName(), _other.m_numberOfEntries), m_value(std::move(_other.m_value))
{
	_other.m_numberOfEntries = 0;
}

ot::GenericDataStructSingle::~GenericDataStructSingle()
{
}

void ot::GenericDataStructSingle::setValue(const ot::Variable& _value)
{
	m_value = _value;
}

void ot::GenericDataStructSingle::setValue(ot::Variable&& _value)
{
	m_value = std::move(_value);
}

const ot::Variable& ot::GenericDataStructSingle::getValue() const
{
	return m_value;
}

void ot::GenericDataStructSingle::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	GenericDataStruct::addToJsonObject(_object, _allocator);
	VariableToJSONConverter converter;
	_object.AddMember("Value", converter(m_value, _allocator), _allocator);
}

void ot::GenericDataStructSingle::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	GenericDataStruct::setFromJsonObject(_object);
	JSONToVariableConverter converter;
	m_value = converter(_object["Value"]);
}

