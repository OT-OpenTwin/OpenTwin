//! @file SessionInformation.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

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