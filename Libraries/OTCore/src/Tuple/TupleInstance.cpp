// @otlicense

// OpenTwin header
#include "OTCore/Tuple/TupleInstance.h"

ot::TupleInstance::TupleInstance(const std::string& _tupleTypeName)
	: m_tupleTypeName(_tupleTypeName)
{
}

void ot::TupleInstance::setTupleTypeName(const std::string& _tupleTypeName)
{
	m_tupleTypeName = _tupleTypeName;
}

void ot::TupleInstance::setTupleFormatName(const std::string& _formatName)
{
	m_tupleFormatName = _formatName;
}

void ot::TupleInstance::setTupleUnits(const std::vector<std::string>& _units)
{
	m_tupleUnits = _units;
}

void ot::TupleInstance::setTupleElementDataTypes(const std::vector<std::string>& _dataTypes)
{
	m_tupleElementDataTypes = _dataTypes;
}

void ot::TupleInstance::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	_jsonObject.AddMember("TupleTypeName", JsonString(m_tupleTypeName, _allocator), _allocator);
	_jsonObject.AddMember("TupleFormatName", JsonString(m_tupleFormatName, _allocator), _allocator);
	
	JsonArray unitsArray;
	for (const std::string& unit : m_tupleUnits)
	{
		unitsArray.PushBack(JsonString(unit, _allocator), _allocator);
	}
	_jsonObject.AddMember("TupleUnits", unitsArray, _allocator);
	
	JsonArray dataTypesArray;
	for (const std::string& dataType : m_tupleElementDataTypes)
	{
		dataTypesArray.PushBack(JsonString(dataType, _allocator), _allocator);
	}
	_jsonObject.AddMember("TupleElementDataTypes", dataTypesArray, _allocator);
}

void ot::TupleInstance::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	m_tupleTypeName = json::getString(_jsonObject, "TupleTypeName");
	m_tupleFormatName = json::getString(_jsonObject, "TupleFormatName");
	if (json::exists(_jsonObject, "TupleUnits"))
	{
		m_tupleUnits = json::getStringVector(_jsonObject, "TupleUnits");
	}
	if (json::exists(_jsonObject, "TupleElementDataTypes"))
	{
		m_tupleElementDataTypes = json::getStringVector(_jsonObject, "TupleElementDataTypes");
	}
}

bool ot::TupleInstance::operator==(const TupleInstance& _other) const
{
	return m_tupleTypeName == _other.m_tupleTypeName &&
		m_tupleFormatName == _other.m_tupleFormatName &&
		m_tupleUnits == _other.m_tupleUnits &&
		m_tupleElementDataTypes == _other.m_tupleElementDataTypes;
}

bool ot::TupleInstance::operator!=(const TupleInstance& _other) const
{
	return !(*this == _other);
}
