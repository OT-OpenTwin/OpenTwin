#include "OTCore/ValueComparisionDefinition.h"

void ValueComparisionDefinition::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("Comperator", ot::JsonString(m_comparator, _allocator), _allocator);
	_object.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Value", ot::JsonString(m_value, _allocator), _allocator);
	_object.AddMember("Type", ot::JsonString(m_type, _allocator), _allocator);
	_object.AddMember("Unit", ot::JsonString(m_unit, _allocator), _allocator);
}

void ValueComparisionDefinition::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_comparator = ot::json::getString(_object, "Comperator");
	m_name = ot::json::getString(_object, "Name");
	m_value = ot::json::getString(_object, "Value");
	m_type = ot::json::getString(_object, "Type");
	m_unit = ot::json::getString(_object, "Unit");
}
