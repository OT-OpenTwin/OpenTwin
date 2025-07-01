#pragma once
#include "OTCore/GenericDataStruct.h"
#include <list>
#include <memory>

class PortData
{
public:
	PortData(const std::string& portName, const std::string& _serialisedData, bool _modified);
	PortData(PortData&& other) noexcept = default;
	PortData(const PortData& other) = delete;
	PortData& operator=(PortData&& other) noexcept = default;
	PortData& operator=(const PortData& other) = delete;
	~PortData() = default;

	void overrideValues(const std::string& _serialisedData);
	const ot::JsonValue& getValues() const;
	const bool getModified() const;
	const std::string& getPortName() { return m_portName; }

private:
	std::string m_portName;
	bool m_modified = false;
	bool m_singleValue = false;
	ot::JsonDocument m_values;
};
