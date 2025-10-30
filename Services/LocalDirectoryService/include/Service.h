// @otlicense

#pragma once

// LDS header
#include "RequestedService.h"

// OpenTwin header
#include "OTSystem/Network.h"
#include "OTSystem/SystemProcess.h"
#include "OTCommunication/ServiceInitData.h"

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
	Service(ServiceManager* _owner, RequestedService&& _requestedService);
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
	ot::RunResult shutdown();

	//! @brief Will check if the service process is still running.
	ot::RunResult checkAlive();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter/Getter

	ServiceStartupData& getStartupData() { return m_counters; };
	const ServiceStartupData& getStartupData() const { return m_counters; };

	ot::ServiceInitData& getInfo() { return m_initData; };
	const ot::ServiceInitData& getInfo() const { return m_initData; };

	void setShuttingDown(bool _isShuttingDown) { m_isShuttingDown = _isShuttingDown; };
	bool isShuttingDown() const { return m_isShuttingDown; };

	const std::string& getUrl() const { return m_url; };
	ot::port_t getPort() const { return m_port; };

	const std::string& getWebsocketUrl() const { return m_websocketUrl; };
	ot::port_t getWebsocketPort() const { return m_websocketPort; };

	void resetIniAttempt() { m_counters.resetIniAttempt(); };
	unsigned int incrIniAttempt() { return m_counters.incrIniAttempt(); };
	unsigned int getIniAttempt() const { return m_counters.getIniAttempt(); };

	void resetStartCounter() { m_counters.resetStartCounter(); };
	unsigned int incrStartCounter() { return m_counters.incrStartCounter(); };
	unsigned int getStartCounter() const { return m_counters.getStartCounter(); };

private:
	ServiceStartupData  m_counters;
	ot::ServiceInitData m_initData;

	ServiceManager*     m_owner;
	bool                m_isShuttingDown;
	std::string			m_url;
	ot::port_t			m_port;
	std::string			m_websocketUrl;
	ot::port_t			m_websocketPort;
	OT_PROCESS_HANDLE	m_processHandle;
};