#pragma once

#include "ServiceStartupInformation.h"

#include <list>
#include <mutex>

class Application;
namespace std { class thread; }

class StartupDispatcher {
public:
	StartupDispatcher(void);
	virtual ~StartupDispatcher();

	// ####################################################################################################################

	// Request management

	void addRequest(const ServiceStartupInformation& _info);
	void addRequest(const std::list<ServiceStartupInformation>& _info);

	// ####################################################################################################################

	void run(void);
	void stop(void);

	// ####################################################################################################################
private:
	void workerFunction(void);
	void serviceStartRequestFailed(const ServiceStartupInformation& _serviceInfo);

	std::list<ServiceStartupInformation>	m_requests;
	std::mutex								m_mutex;
	std::thread *							m_workerThread;
	bool									m_isStopping;

	StartupDispatcher(StartupDispatcher&) = delete;
	StartupDispatcher& operator = (StartupDispatcher&) = delete;
};