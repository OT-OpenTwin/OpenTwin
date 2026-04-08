// @otlicense

// OpenTwin header
#include "OTCore/DatasetDependencyInfo.h"

ot::DatasetDependencyInfo::DatasetDependencyInfo(const ConstJsonObject& _jsonObject)
	: DatasetDependencyInfo()
{
	setFromJsonObject(_jsonObject);
}

void ot::DatasetDependencyInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	_jsonObject.AddMember("Label", JsonString(m_label, _allocator), _allocator);
	_jsonObject.AddMember("Unit", JsonString(m_unit, _allocator), _allocator);
	_jsonObject.AddMember("Value", JsonString(m_value, _allocator), _allocator);
}

void ot::DatasetDependencyInfo::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	m_label = json::getString(_jsonObject, "Label");
	m_unit = json::getString(_jsonObject, "Unit");
	m_value = json::getString(_jsonObject, "Value");
}

bool ot::DatasetDependencyInfo::operator==(const DatasetDependencyInfo& _other) const
{
	return m_label == _other.m_label && m_unit == _other.m_unit && m_value == _other.m_value;
}
