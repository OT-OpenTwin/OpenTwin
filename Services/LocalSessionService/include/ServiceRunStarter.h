#pragma once

// Service header
#include "Service.h"
#include "Session.h"

// OT header
#include "OTCore/CoreTypes.h"

// std header
#include <mutex>
#include <list>

class ServiceRunStarter {
public:
	static ServiceRunStarter& instance();

	void addService(const Session& _session, const Service& _service);
	void sessionClosing(const std::string& _sessionId);

private:
	struct StartupInformation {
		std::string sessionId;

		std::string serviceName;
		std::string serviceType;
		std::string serviceUrl;
		ot::serviceID_t serviceId;

		std::string credentialsUserName;
		std::string credentialsUserPassword;

		std::string databaseUserName;
		std::string databaseUserPassword;

		std::string userCollection;
		std::string sessionType;
	};

	std::mutex						m_mutex;
	bool							m_isStopping;
	bool							m_isRunning;
	std::list<StartupInformation>	m_queue;

	ServiceRunStarter();
	virtual ~ServiceRunStarter();

	bool queueEmpty(void);
	void worker(void);
	bool sendRunMessageToService(const StartupInformation& _info, const std::string& _message);

};