// @otlicense

// OpenTwin header
#include "OTCore/MetadataHandle/MetadataParameter.h"

MetadataParameter::MetadataParameter(const MetadataParameter& _other)
	: parameterName(_other.parameterName), parameterLabel(_other.parameterLabel), parameterUID(_other.parameterUID),
	unit(_other.unit), typeName(_other.typeName), values(_other.values)
{
	metaData.CopyFrom(_other.metaData, metaData.GetAllocator());
}

MetadataParameter::MetadataParameter(MetadataParameter&& _other) noexcept
{
	swap(*this, _other);
}

MetadataParameter& MetadataParameter::operator=(const MetadataParameter& _other)
{
	MetadataParameter temp(_other);
	swap(*this, temp);
	return *this;
}

MetadataParameter& MetadataParameter::operator=(MetadataParameter&& _other)
{
	MetadataParameter temp(_other);
	swap(*this, temp);
	return *this;
}

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

void MetadataParameter::swap(MetadataParameter& _origin, MetadataParameter& _target) noexcept
{
	std::swap(_origin.parameterName, _target.parameterName);
	std::swap(_origin.parameterLabel, _target.parameterLabel);
	std::swap(_origin.parameterUID, _target.parameterUID);
	std::swap(_origin.unit, _target.unit);
	std::swap(_origin.typeName, _target.typeName);
	std::swap(_origin.values, _target.values);
	_origin.metaData.Swap(_target.metaData);
}
