#include "PortData.h"

PortData::PortData(const std::string& _portName, const std::string& _serialisedData, bool _modified )
	:m_portName(_portName), m_modified(_modified)
{
	m_values.fromJson(_serialisedData);
}

PortData::PortData(PortData&& other) noexcept
	:m_portName(other.m_portName), m_modified(other.getModified()), m_values(std::move(other.m_values))
{
}

PortData& PortData::operator=(PortData&& other) noexcept
{
	m_portName = (other.m_portName);
	m_values = (std::move(other.m_values));
	m_modified = other.getModified();
	return *this;
}

PortData::~PortData()
{
	
}

void PortData::overrideValues(const std::string&  _serialisedData)
{
	m_values.fromJson(_serialisedData);
	m_modified = true;
}

const ot::JsonDocument& PortData::getValues() const
{
	return m_values;
}

const bool PortData::getModified() const
{
	return m_modified;
}

