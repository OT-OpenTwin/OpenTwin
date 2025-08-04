#pragma once
#include "OTCore/GenericDataStruct.h"
#include <list>
#include <memory>

class PortData
{
public:
	PortData(const std::string& portName, const std::string& _serialisedData, const std::string& _serialisedMetaData, bool _modified);
	PortData(PortData&& other) noexcept = default;
	PortData(const PortData& other) = delete;
	PortData& operator=(PortData&& other) noexcept = default;
	PortData& operator=(const PortData& other) = delete;
	~PortData() = default;

	void overrideData(const std::string& _serialisedData);
	void overrideMetaData(const std::string& _serialisedMetaData);
	const ot::JsonValue& getData() const;
	const ot::JsonValue& getMetadata() const;
	const ot::JsonValue& getDataAndMetadata() const { return m_data; }
	const bool getModified() const { return m_modified; }
	const std::string& getPortName() { return m_portName; }

private:
	std::string m_portName;
	bool m_modified = false;
	bool m_singleValue = false;
	ot::JsonDocument m_data;
};
