#pragma once

// OT header
#include "OpenTwinCore/CoreTypes.h"

// std header
#include <mutex>
#include <list>

class Service;
class Session;

class ServiceRunStarter {
public:
	static ServiceRunStarter& instance();

	void addService(Session * _session, Service * _service);
	void sessionClosing(const std::string& _sessionId);

private:
	ServiceRunStarter();
	virtual ~ServiceRunStarter();

	bool queueEmpty(void);
	void worker(void);

	struct startupInformation {
		std::string sessionId;

		std::string serviceName;
		std::string serviceType;
		std::string serviceUrl;
		ot::serviceID_t serviceId;

		std::string credentialsUserName;
		std::string credentialsUserPassword;
		
		std::string userCollection;
	};

	std::mutex						m_mutex;
	bool							m_isStopping;
	bool							m_isRunning;
	std::list<startupInformation>	m_queue;
};