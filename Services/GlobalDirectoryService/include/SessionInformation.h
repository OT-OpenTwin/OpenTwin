//! @file SessionInformation.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"

// std header
#include <string>

//! @brief The SessionInformation class holds information about a session, such as its ID and the URL of the session service.
class SessionInformation {
public:
	SessionInformation();
	SessionInformation(const std::string& _sessionId, const std::string& _sessionServiceURL);
	SessionInformation(const SessionInformation& _other);
	SessionInformation(SessionInformation&& _other) noexcept;
	virtual ~SessionInformation();

	SessionInformation& operator = (const SessionInformation& _other);
	SessionInformation& operator = (SessionInformation&& _other) noexcept;

	bool operator == (const SessionInformation& _other) const;
	bool operator != (const SessionInformation& _other) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setID(const std::string& _id) { m_id = _id; }
	const std::string& getId(void) const { return m_id; }

	void setSessionServiceURL(const std::string& _url) { m_sessionServiceURL = _url; }
	const std::string& getSessionServiceURL(void) const { return m_sessionServiceURL; }

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const;

private:
	std::string m_id; //! @brief The session ID.
	std::string m_sessionServiceURL; //! @brief The URL of the session service managing this session.
};