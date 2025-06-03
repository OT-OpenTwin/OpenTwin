//! @file ServiceInformation.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"

// GDS header
#include "SessionInformation.h"

// std header
#include <string>

//! @brief The ServiceInformation class holds information about a service, such as its name, type, and session information.
class ServiceInformation {
public:
	ServiceInformation();
	ServiceInformation(const std::string& _name, const std::string& _type);
	ServiceInformation(const std::string& _name, const std::string& _type, const SessionInformation& _session);
	ServiceInformation(const std::string& _name, const std::string& _type, const std::string& _sessionId, const std::string& _sessionServiceURL);
	ServiceInformation(const ServiceInformation& _other);
	ServiceInformation(ServiceInformation&& _other) noexcept;
	virtual ~ServiceInformation();

	ServiceInformation& operator = (const ServiceInformation& _other);
	ServiceInformation& operator = (ServiceInformation&& _other) noexcept;

	bool operator == (const ServiceInformation& _other) const;
	bool operator != (const ServiceInformation& _other) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName(void) const { return m_name; };

	void setType(const std::string& _type) { m_type = _type; };
	const std::string& getType(void) const { return m_type; };

	void setSessionInformation(const SessionInformation& _session) { m_session = _session; };
	const SessionInformation& getSessionInformation(void) const { return m_session; };

	void setSessionId(const std::string& _sessionId) { m_session.setID(_sessionId); };
	const std::string& getSessionId(void) const { return m_session.getId(); };

	void setSessionServiceURL(const std::string& _sessionServiceURL) { m_session.setSessionServiceURL(_sessionServiceURL); };
	const std::string& getSessionServiceURL(void) const { return m_session.getSessionServiceURL(); };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const;

private:
	std::string m_name; //! @brief The name of the service.
	std::string m_type; //! @brief The type of the service.
	SessionInformation m_session; //! @brief The session information associated with the service.
};