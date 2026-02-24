#include "OTCore/Tuple/TupleInstance.h"

TupleInstance::TupleInstance(const std::string& _tupleTypeName)
	: m_tupleTypeName(_tupleTypeName)
{}

void TupleInstance::setTupleTypeName(const std::string& _tupleTypeName)
{
	m_tupleTypeName = _tupleTypeName;
}

void TupleInstance::setTupleFormatName(const std::string & _formatName)
{
	m_tupleFormatName = _formatName;
}

void TupleInstance::setTupleUnits(const std::vector<std::string>& _units)
{
	m_tupleUnits = _units;
}

void TupleInstance::setTupleElementDataTypes(const std::vector<std::string>& _dataTypes)
{
	m_tupleElementDataTypes = _dataTypes;
}

void TupleInstance::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const
{
	_jsonObject.AddMember("TupleTypeName", ot::JsonString(m_tupleTypeName, _allocator), _allocator);
	_jsonObject.AddMember("TupleFormatName", ot::JsonString(m_tupleFormatName, _allocator), _allocator);
	ot::JsonArray unitsArray;
	for (const std::string& unit : m_tupleUnits)
	{
		unitsArray.PushBack(ot::JsonString(unit, _allocator), _allocator);
	}
	_jsonObject.AddMember("TupleUnits", unitsArray, _allocator);
	ot::JsonArray dataTypesArray;
	for (const std::string& dataType : m_tupleElementDataTypes)
	{
		dataTypesArray.PushBack(ot::JsonString(dataType, _allocator), _allocator);
	}
	_jsonObject.AddMember("TupleElementDataTypes", dataTypesArray, _allocator);
}

void TupleInstance::setFromJsonObject(const ot::ConstJsonObject& _jsonObject)
{
	m_tupleTypeName = ot::json::getString(_jsonObject, "TupleTypeName");
	m_tupleFormatName = ot::json::getString(_jsonObject, "TupleFormatName");
	if (ot::json::exists(_jsonObject, "TupleUnits"))
	{
		m_tupleUnits = ot::json::getStringVector(_jsonObject, "TupleUnits");
	}
	if (ot::json::exists(_jsonObject, "TupleElementDataTypes"))
	{
		m_tupleElementDataTypes = ot::json::getStringVector(_jsonObject, "TupleElementDataTypes");
	}
}

bool TupleInstance::isSingle() const
{
	return m_tupleTypeName.empty();
}

bool TupleInstance::operator==(const TupleInstance& _other) const
{
		return m_tupleTypeName == _other.m_tupleTypeName &&
		m_tupleFormatName == _other.m_tupleFormatName &&
		m_tupleUnits == _other.m_tupleUnits &&
		m_tupleElementDataTypes == _other.m_tupleElementDataTypes;
}
