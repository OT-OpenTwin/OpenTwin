// @otlicense
// File: Service.h
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
#include "OTSystem/Flags.h"
#include "OTSystem/SystemTypes.h"
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/ServiceBase.h"
#include "OTCommunication/LSSDebugInfo.h"

// std header
#include <list>
#include <string>

class Service : public ot::ServiceBase {
	OT_DECL_DEFCOPY(Service)
	OT_DECL_DEFMOVE(Service)
	OT_DECL_NODEFAULT(Service)
public:
	enum ServiceStateFlag {
		NoState      = 0 << 0,
		IsDebug      = 1 << 0, //! @brief Service is in debug mode.
		Requested    = 1 << 1, //! @brief Service is requested by the session, but not yet registered.
		Alive        = 1 << 2, //! @brief Service is registered in the session.
		Running      = 1 << 3, //! @brief Service is running (received run command).
		ShuttingDown = 1 << 4, //! @brief Service is shutting down, waiting for confirmation.
		Hidden       = 1 << 5  //! @brief Service is invisible to other services in the session.s
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

	void setIsDebug(bool _isDebug = true) { m_state.setFlag(Service::IsDebug, _isDebug); };
	bool isDebug() const { return m_state & Service::IsDebug; };

	void setRequested(bool _requested = true) { m_state.setFlag(Service::Requested, _requested); };
	bool isRequested() const { return m_state & Service::Requested; };

	void setAlive(bool _alive = true) { m_state.setFlag(Service::Alive, _alive); };
	bool isAlive() const { return m_state & Service::Alive; };

	void setRunning(bool _running = true) { m_state.setFlag(Service::Running, _running); };
	bool isRunning() const { return m_state & Service::Running; };

	void setShuttingDown(bool _shuttingDown = true) { m_state.setFlag(Service::ShuttingDown, _shuttingDown); };
	bool isShuttingDown() const { return m_state & Service::ShuttingDown; };

	void setHidden(bool _hidden) { m_state.setFlag(Service::Hidden, _hidden); };
	bool isHidden() const { return m_state & Service::Hidden; };

	//! @brief Will return the port numbers used by this service.
	std::list<ot::port_t> getPortNumbers(void) const;

	static std::string debugLogString(const ot::ServiceBase& _serviceInfo, const std::string& _sessionID);
	std::string debugLogString(const std::string& _sessionID) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void getDebugInfo(ot::LSSDebugInfo::ServiceInfo& _info) const;

	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

private:
	std::string      m_connectedType;
	std::string      m_websocketUrl; //! @brief Service websocket url (if available, otherwise empty).
	ServiceState     m_state;        //! @brief Service state.
};

OT_ADD_FLAG_FUNCTIONS(Service::ServiceStateFlag)