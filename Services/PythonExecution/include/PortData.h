// @otlicense
// File: PortData.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
	ot::JsonValue& getDataAndMetadata() { return m_data; }
	const bool getModified() const { return m_modified; }
	const std::string& getPortName() { return m_portName; }

private:
	std::string m_portName;
	bool m_modified = false;
	bool m_singleValue = false;
	ot::JsonDocument m_data;
};
