//! @file SessionInformation.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"

// std header
#include <string>

class SessionInformation {
public:
	SessionInformation();
	SessionInformation(const std::string& _id, const std::string& _sessionServiceUrl);
	SessionInformation(const SessionInformation& _other);
	virtual ~SessionInformation();

	SessionInformation& operator = (const SessionInformation& _other);
	bool operator == (const SessionInformation& _other) const;
	bool operator != (const SessionInformation& _other) const;

	bool operator < (const SessionInformation& _other) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter/Getter

	void setID(const std::string& _id) { m_id = _id; };
	const std::string& getId(void) const { return m_id; };

	void setSessionServiceURL(const std::string& _url) { m_sessionServiceUrl = _url; };
	const std::string& getSessionServiceURL(void) const { return m_sessionServiceUrl; };

private:
	std::string		m_id;
	std::string		m_sessionServiceUrl;
};