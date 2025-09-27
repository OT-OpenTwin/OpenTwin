//! @file GSSRegistrationInfo.h
//! @author Alexander Kuester (alexk95)
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

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

	void reset(void);

	void setServiceID(ot::serviceID_t _id) { m_serviceID = _id; };
	ot::serviceID_t getServiceID(void) const { return m_serviceID; };

	void setDataBaseURL(const std::string& _url) { m_databaseURL = _url; };
	const std::string& getDataBaseURL(void) const { return m_databaseURL; };

	void setAuthURL(const std::string& _url) { m_authURL = _url; };
	const std::string& getAuthURL(void) const { return m_authURL; };

	void setGdsURL(const std::string& _url) { m_gdsURL = _url; };
	const std::string& getGdsURL(void) const { return m_gdsURL; };

	void setLogFlags(const ot::LogFlags& _flags) { m_logFlags = _flags; };
	const ot::LogFlags& getLogFlags(void) const { return m_logFlags; };
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator);

private:
	ot::serviceID_t    m_serviceID;
	std::string        m_databaseURL;
	std::string        m_authURL;
	std::string        m_gdsURL;
	ot::LogFlags       m_logFlags;
};