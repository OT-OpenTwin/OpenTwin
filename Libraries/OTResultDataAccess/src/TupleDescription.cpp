#include "OTResultDataAccess/SerialisationInterfaces/TupleDescription.h"

bool TupleDescription::operator==(const TupleDescription& _other) const
{
	return m_name == _other.m_name &&
		m_formatName == _other.m_formatName &&
		m_tupleElementNames == _other.m_tupleElementNames &&
		m_dataType == _other.m_dataType &&
		m_tupleUnits == _other.m_tupleUnits;
}

void TupleDescription::setFormatName(const std::string& _formatName)
{
	m_formatName = _formatName;
	initialiseTupleElementNames();
}

void TupleDescription::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const
{
	_jsonObject.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
	_jsonObject.AddMember("FormatName", ot::JsonString(m_formatName, _allocator), _allocator);
	_jsonObject.AddMember("DataType", ot::JsonString(m_dataType, _allocator), _allocator);
	_jsonObject.AddMember("Units", ot::JsonArray(m_tupleUnits, _allocator), _allocator);
}

void TupleDescription::setFromJsonObject(const ot::ConstJsonObject& _jsonObject)
{
	m_name = ot::json::getString(_jsonObject, "Name");
	m_formatName = ot::json::getString(_jsonObject, "FormatName");
	m_dataType = ot::json::getString(_jsonObject, "DataType");
	m_tupleUnits = ot::json::getStringVector(_jsonObject, "Units");
}

