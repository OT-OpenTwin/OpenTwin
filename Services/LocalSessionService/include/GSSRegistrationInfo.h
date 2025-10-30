// @otlicense
// File: GSSRegistrationInfo.h
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
#include "OTCore/JSON.h"
#include "OTCore/LogTypes.h"
#include "OTCore/ServiceBase.h"

class GSSRegistrationInfo {
	OT_DECL_DEFCOPY(GSSRegistrationInfo)
	OT_DECL_DEFMOVE(GSSRegistrationInfo)
public:
	GSSRegistrationInfo();
	~GSSRegistrationInfo() {};

	void reset();

	void setServiceID(ot::serviceID_t _id) { m_serviceID = _id; };
	ot::serviceID_t getServiceID() const { return m_serviceID; };

	void setDataBaseURL(const std::string& _url) { m_databaseURL = _url; };
	const std::string& getDataBaseURL() const { return m_databaseURL; };

	void setAuthURL(const std::string& _url) { m_authURL = _url; };
	const std::string& getAuthURL() const { return m_authURL; };

	void setGdsURL(const std::string& _url) { m_gdsURL = _url; };
	const std::string& getGdsURL() const { return m_gdsURL; };

	void setLMSURL(const std::string& _url) { m_lmsURL = _url; };
	const std::string& getLMSURL() const { return m_lmsURL; };

	void setLoggingURL(const std::string& _url) { m_loggingURL = _url; };
	const std::string& getLoggingURL() const { return m_loggingURL; };

	void setLogFlags(const ot::LogFlags& _flags) { m_logFlags = _flags; };
	const ot::LogFlags& getLogFlags() const { return m_logFlags; };
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator);

private:
	ot::serviceID_t    m_serviceID;
	std::string        m_databaseURL;
	std::string        m_authURL;
	std::string        m_gdsURL;
	std::string        m_loggingURL;
	std::string        m_lmsURL;
	ot::LogFlags       m_logFlags;
};