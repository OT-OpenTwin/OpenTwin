#include "OTCore/GenericDataStruct.h"

void ot::GenericDataStruct::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("numberOfEntries", ot::JsonValue(m_numberOfEntries), _allocator);
	_object.AddMember("ClassName", ot::JsonString(m_typeName, _allocator), _allocator);
}

void ot::GenericDataStruct::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_numberOfEntries = ot::json::getUInt(_object, "numberOfEntries");
	m_typeName = ot::json::getString(_object, "ClassName");
}
