#pragma once

#include "OpenTwinCore/ServiceBase.h"

class SessionService;
namespace std { class thread; }

class GlobalSessionService : public ot::ServiceBase {
public:
	enum ConnectionStatus {
		Connected,
		ConnectionFailed
	};

	GlobalSessionService(const std::string& _url, SessionService * _owner);
	virtual ~GlobalSessionService();

	void startHealthCheck(void);

	void stopHealthCheck(bool _joinThread);

private:

	void healthCheck(void);

	SessionService *		m_owner;
	bool					m_healthCheckRunning;
	ConnectionStatus		m_connectionStatus;
	std::thread *			m_workerThread;

	GlobalSessionService() = delete;
	GlobalSessionService(GlobalSessionService&) = delete;
	GlobalSessionService& operator = (GlobalSessionService&) = delete;
};