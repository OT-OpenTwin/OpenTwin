//! @file Service.h
//! @authors Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTSystem/SystemTypes.h"
#include "OTCore/JSON.h"
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
		Alive        = 1 << 2, //! @brief Service is registered in the session.
		ShuttingDown = 1 << 3  //! @brief Service is shutting down, waiting for confirmation.
	};
	typedef ot::Flags<ServiceStateFlag> ServiceState; //! @brief Flags used to describe the state of the service.

	//! @brief Constructor.
	Service(const ot::ServiceBase& _serviceInfo, const std::string& _sessionId);

	//! @brief Destructor.
	virtual ~Service();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setWebsocketUrl(const std::string& _websocketUrl) { m_websocketUrl = _websocketUrl; };
	const std::string& getWebsocketUrl(void) const { return m_websocketUrl; };

	void setIsDebug(bool _isDebug) { m_state.setFlag(Service::IsDebug, _isDebug); };
	bool isDebug() const { return m_state & Service::IsDebug; };

	void setRequested(bool _requested) { m_state.setFlag(Service::Requested, _requested); };
	bool isRequested() const { return m_state & Service::Requested; };

	void setAlive(bool _alive) { m_state.setFlag(Service::Alive, _alive); };
	bool isAlive() const { return m_state & Service::Alive; };

	void setShuttingDown(bool _shuttingDown) { m_state.setFlag(Service::ShuttingDown, _shuttingDown); };
	bool isShuttingDown() const { return m_state & Service::ShuttingDown; };

	//! @brief Will return the port numbers used by this service.
	std::list<ot::port_t> getPortNumbers(void) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

private:
	std::string      m_websocketUrl; //! @brief Service websocket url (if available, otherwise empty).
	ServiceState     m_state;        //! @brief Service state.
};

OT_ADD_FLAG_FUNCTIONS(Service::ServiceStateFlag)