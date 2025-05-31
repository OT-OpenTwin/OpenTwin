//! @file Service.h
//! @authors Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/SystemTypes.h"
#include "OTCore/JSON.h"
#include "OTCore/Flags.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/ServiceBase.h"

// std header
#include <list>
#include <string>

class Service : public ot::ServiceBase {
public:
	enum ServiceStateFlag {
		NoState      = 0 << 0,
		IsDebug      = 1 << 0, //! @brief Service is in debug mode.
		Requested    = 1 << 1, //! @brief Service is requested by the session, but not yet registered.
		Visible      = 1 << 2, //! @brief Service is visible to other services in the same session.
		ShuttingDown = 1 << 3  //! @brief Service is shutting down, no new connections are allowed.
	};
	typedef ot::Flags<ServiceStateFlag> ServiceState; //! @brief Flags used to describe the state of the service.

	//! @brief Constructor.
	Service(const ot::ServiceBase& _serviceInfo, const std::string& _sessionId);

	//! @brief Destructor.
	virtual ~Service();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setID(ot::serviceID_t _id) { m_id = _id; };
	ot::serviceID_t getId(void) const { return m_id; };

	void setURL(const std::string& _url) { m_url = _url; };
	const std::string& getUrl(void) const { return m_url; };

	void setWebsocketUrl(const std::string& _websocketUrl) { m_websocketUrl = _websocketUrl; };
	const std::string& getWebsocketUrl(void) const { return m_websocketUrl; };

	const std::string& getName(void) const { return m_name; };
	const std::string& getType(void) const { return m_type; };

	void setVisible(bool _visible) { m_state.setFlag(Service::Visible, _visible); };
	bool isVisible() const { return m_state & Service::Visible; };

	void setShuttingDown(bool _shuttingDown) { m_state.setFlag(Service::ShuttingDown, _shuttingDown); };
	bool isShuttingDown() const { return m_state & Service::ShuttingDown; };

	void setIsDebug(bool _isDebug) { m_state.setFlag(Service::IsDebug, _isDebug); };
	bool isDebug() const { return m_state & Service::IsDebug; };

	//! @brief Will return the port numbers used by this service.
	std::list<ot::port_t> getPortNumbers(void) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

private:
	ot::serviceID_t  m_id;           //! @brief Service ID.
	std::string      m_url;          //! @brief Service url.
	std::string      m_websocketUrl; //! @brief Service websocket url (if available, otherwise empty).
	std::string      m_name;         //! @brief Service name.
	std::string      m_type;         //! @brief Service type.
	ServiceState     m_state;        //! @brief Service state.

	std::string      m_sessionId;    //! @brief The session ID this service is registered in.
};

OT_ADD_FLAG_FUNCTIONS(Service::ServiceStateFlag)