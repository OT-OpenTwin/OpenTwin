#include "PortData.h"

PortData::PortData(const std::string& _portName, const std::string& _serialisedData, bool _modified )
	:m_portName(_portName), m_modified(_modified)
{
	m_values.fromJson(_serialisedData);
	if (m_values.Empty())
	{
		m_values.AddMember("__internal__", ot::JsonString(_serialisedData, m_values.GetAllocator()), m_values.GetAllocator());
		m_singleValue = true;
	}
}

void PortData::overrideValues(const std::string&  _serialisedData)
{
	m_values.fromJson(_serialisedData);
	m_modified = true;
}

const ot::JsonValue& PortData::getValues() const
{
	if (m_singleValue)
	{
		assert(ot::json::exists(m_values, "__internal__"));
		return m_values["__internal__"];
	}
	else
	{
		return m_values;
	}
}

const bool PortData::getModified() const
{
	return m_modified;
}

