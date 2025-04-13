#pragma once

#include "OTCore/ServiceBase.h"

namespace std { class thread; }

class GlobalSessionService : public ot::ServiceBase {
public:
	enum ConnectionStatus {
		Connected,
		ConnectionFailed
	};

	GlobalSessionService(const std::string& _url);
	virtual ~GlobalSessionService();

	bool confirmSession(const std::string& _sessionId, const std::string& _userName);

	void startHealthCheck(void);

	void stopHealthCheck(bool _joinThread);

private:

	void healthCheck(void);

	bool					m_healthCheckRunning;
	ConnectionStatus		m_connectionStatus;
	std::thread *			m_workerThread;

	GlobalSessionService() = delete;
	GlobalSessionService(GlobalSessionService&) = delete;
	GlobalSessionService& operator = (GlobalSessionService&) = delete;
};