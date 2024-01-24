#include "OTCore/GenericDataStruct.h"

void ot::GenericDataStruct::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("numberOfEntries", ot::JsonValue(_numberOfEntries), _allocator);
	_object.AddMember("ClassName", ot::JsonString(_typeName, _allocator), _allocator);
}

void ot::GenericDataStruct::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	_numberOfEntries = ot::json::getUInt(_object, "numberOfEntries");
	_typeName = ot::json::getString(_object, "ClassName");
}
