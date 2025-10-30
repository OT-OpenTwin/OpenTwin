// @otlicense
// File: SessionInformation.h
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

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/ServiceInitData.h"

// std header
#include <string>

class SessionInformation {
	OT_DECL_DEFCOPY(SessionInformation)
	OT_DECL_DEFMOVE(SessionInformation)
public:
	SessionInformation() = default;
	SessionInformation(const ot::ServiceInitData& _serviceInitData);
	~SessionInformation() {};

	bool operator == (const SessionInformation& _other) const;
	bool operator != (const SessionInformation& _other) const;

	bool operator < (const SessionInformation& _other) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter/Getter

	void setID(const std::string& _id) { m_id = _id; };
	const std::string& getId() const { return m_id; };

	void setSessionServiceURL(const std::string& _url) { m_sessionServiceUrl = _url; };
	const std::string& getSessionServiceURL() const { return m_sessionServiceUrl; };

private:
	std::string		m_id;
	std::string		m_sessionServiceUrl;
};