//! @file ServiceInformation.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// GDS header
#include "SessionInformation.h"

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <string>

//! @brief The ServiceInformation class holds information about a service, such as its name, type, and session information.
class ServiceInformation {
	OT_DECL_DEFCOPY(ServiceInformation)
	OT_DECL_DEFMOVE(ServiceInformation)
public:
	ServiceInformation();
	ServiceInformation(const std::string& _name, const std::string& _type, ot::serviceID_t _serviceId, const SessionInformation& _session);
	ServiceInformation(const std::string& _name, const std::string& _type, ot::serviceID_t _serviceId, const std::string& _sessionId, const std::string& _sessionServiceURL);
	virtual ~ServiceInformation();

	bool operator == (const ServiceInformation& _other) const;
	bool operator != (const ServiceInformation& _other) const;

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName(void) const { return m_name; };

	void setType(const std::string& _type) { m_type = _type; };
	const std::string& getType(void) const { return m_type; };

	void setID(ot::serviceID_t _id) { m_id = _id; };
	ot::serviceID_t getID(void) const { return m_id; };

	void setSessionInformation(const SessionInformation& _session) { m_session = _session; };
	const SessionInformation& getSessionInformation(void) const { return m_session; };

	void setSessionId(const std::string& _sessionId) { m_session.setID(_sessionId); };
	const std::string& getSessionId(void) const { return m_session.getId(); };

	void setSessionServiceURL(const std::string& _sessionServiceURL) { m_session.setSessionServiceURL(_sessionServiceURL); };
	const std::string& getSessionServiceURL(void) const { return m_session.getSessionServiceURL(); };

private:
	std::string m_name; //! @brief Service name.
	std::string m_type; //! @brief Service type.
	ot::serviceID_t m_id; //! @brief Service ID.
	SessionInformation m_session; //! @brief The session information associated with the service.
};