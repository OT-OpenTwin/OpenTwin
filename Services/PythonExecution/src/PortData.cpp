#include "PortData.h"

PortData::PortData(const std::string& _portName, const std::string& _serialisedData, const std::string& _serialisedMetaData, bool _modified)
	:m_portName(_portName), m_modified(_modified)
{
	ot::JsonDocument data;
	data.fromJson(_serialisedData);
	ot::JsonValue data_copy;
	data_copy.CopyFrom(data.Move(), m_data.GetAllocator());
	m_data.AddMember("Data", data_copy, m_data.GetAllocator());

	ot::JsonDocument metadata;
	metadata.fromJson(_serialisedMetaData);
	ot::JsonValue metadata_Copy;
	metadata_Copy.CopyFrom(metadata.Move(), m_data.GetAllocator());
	m_data.AddMember("Meta", metadata_Copy, m_data.GetAllocator());
}

void PortData::overrideData(const std::string&  _serialisedData)
{
	ot::JsonDocument data;
	data.fromJson(_serialisedData);
	ot::JsonValue data_copy;
	data_copy.CopyFrom(data.Move(), m_data.GetAllocator());
	m_data["Data"] = data_copy;
	m_modified= true;
}

void PortData::overrideMetaData(const std::string& _serialisedMetaData)
{
	ot::JsonDocument metaData;
	metaData.fromJson(_serialisedMetaData);
	ot::JsonValue metaData_copy;
	metaData_copy.CopyFrom(metaData.Move(), m_data.GetAllocator());
	m_data["Meta"] = metaData_copy;
	m_modified= true;
}

const ot::JsonValue& PortData::getData() const
{
	return m_data["Data"];
}

const ot::JsonValue& PortData::getMetadata() const
{
	return m_data["Meta"];
}