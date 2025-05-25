//! @file Service.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LDS header
#include "ServiceInformation.h"
#include "SessionInformation.h"

// OpenTwin header
#include "OTSystem/Network.h"
#include "OTSystem/SystemProcess.h"

// std header
#include <map>
#include <list>
#include <string>
#include <thread>
#include <stdint.h>

class ServiceManager;

class Service {
	OT_DECL_NOCOPY(Service)
	OT_DECL_NODEFAULT(Service)
public:
	Service(ServiceManager * _owner, const ServiceInformation& _info);
	Service(Service&& _other) noexcept;
	virtual ~Service();

	Service& operator = (Service&& _other) noexcept;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	//! @brief Add the object contents to the provided JSON object
	//! @param _document The JSON document (used to get the allocator)
	//! @param _object The JSON object to add the contents to
	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Service control

	//! @brief Will start the service.
	//! @param _url Service URL.
	//! @param _port Service port.
	//! @param _websocketPort Websocket port. The websocket port is only used when launching a relay service.
	//! @return Returns the RunResult of ot::SystemProcess::runApplication().
	ot::RunResult run(const std::string& _url, ot::port_t _port, ot::port_t _websocketPort = 0);

	//! @brief Will terminate the service process.
	ot::RunResult shutdown(void);

	//! @brief Will check if the service process is still running.
	ot::RunResult checkAlive(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter/Getter

	const ServiceInformation& getInfo(void) const { return m_info; };

	const std::string& getUrl(void) const { return m_url; };
	ot::port_t getPort(void) const { return m_port; };

	const std::string& getWebsocketUrl(void) const { return m_websocketUrl; };
	ot::port_t getWebsocketPort(void) const { return m_websocketPort; };

	void resetIniAttempt(void) { m_info.resetIniAttempt(); };
	void incrIniAttempt(void) { m_info.incrIniAttempt(); };
	unsigned int getIniAttempt(void) const { return m_info.getIniAttempt(); };

	void resetStartCounter(void) { m_info.resetStartCounter(); };
	void incrStartCounter(void) { m_info.incrStartCounter(); };
	unsigned int getStartCounter(void) const { return m_info.getStartCounter(); };

private:
	ServiceInformation	m_info;
	
	ServiceManager *	m_owner;
	bool				m_isAlive;
	std::string			m_url;
	ot::port_t			m_port;
	std::string			m_websocketUrl;
	ot::port_t			m_websocketPort;
	OT_PROCESS_HANDLE	m_processHandle;
};
