#pragma once

#include "ServiceInformation.h"
#include "SessionInformation.h"

#include <string>
#include <list>
#include <map>
#include <thread>

#include "OTSystem/Application.h"
#include "OTSystem/Network.h"
#include <stdint.h>
class ServiceManager;

class Service
{
public:
	Service(ServiceManager * _owner, const ServiceInformation& _info);
	virtual ~Service();

	//! @brief Add the object contents to the provided JSON object
	//! @param _document The JSON document (used to get the allocator)
	//! @param _object The JSON object to add the contents to
	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

	ot::app::RunResult run(const SessionInformation& _sessionInformation, const std::string& _url, ot::port_t _port, ot::port_t _websocketPort = 0);
	ot::app::RunResult shutdown(void);

	ot::app::RunResult checkAlive();
	void incrStartCounter(void);

	const ServiceInformation& information(void) const { return m_info; };
	unsigned int startCounter(void) const { return m_startCounter; };
	const std::string& url(void) const { return m_url; };
	ot::port_t port(void) const { return m_port; };
	const std::string& websocketUrl(void) const { return m_websocketUrl; };
	ot::port_t websocketPort(void) const { return m_websocketPort; };


private:
	ServiceInformation	m_info;
	unsigned int		m_startCounter;
	ServiceManager *	m_owner;
	bool				m_isAlive;
	std::string			m_url;
	ot::port_t			m_port;
	std::string			m_websocketUrl;
	ot::port_t			m_websocketPort;
	OT_PROCESS_HANDLE	m_processHandle;
};
