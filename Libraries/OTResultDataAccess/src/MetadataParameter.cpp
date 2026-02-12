// @otlicense

// OpenTwin header
#include "OTResultDataAccess/MetadataHandle/MetadataParameter.h"

void MetadataParameter::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Label", ot::JsonString(parameterLabel, _allocator), _allocator);
	_object.AddMember("Name", ot::JsonString(parameterName, _allocator), _allocator);
	_object.AddMember("Type", ot::JsonString(typeName, _allocator), _allocator);
	_object.AddMember("Unit", ot::JsonString(unit, _allocator), _allocator);
	ot::VariableToJSONConverter converter;
	ot::JsonValue array = converter(values, _allocator);
	_object.AddMember("Values", array, _allocator);
}

void MetadataParameter::setFromJsonObject(const ot::ConstJsonObject& _object) {
	parameterLabel = ot::json::getString(_object, "Label");
	parameterName = ot::json::getString(_object, "Name");
	unit = ot::json::getString(_object, "Unit");
	typeName = ot::json::getString(_object, "Type");
	ot::ConstJsonArray array = ot::json::getArray(_object, "Values");
	ot::JSONToVariableConverter converter;
	values = converter(array);
}