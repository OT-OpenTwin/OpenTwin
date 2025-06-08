//! @file GlobalSessionService.h
//! @author Alexander Kuester (alexk95)
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LSS header
#include "GSSRegistrationInfo.h"

// OpenTwin header
#include "OTCore/ServiceBase.h"

// std header
#include <mutex>

namespace std { class thread; }

class GlobalSessionService : public ot::ServiceBase {
public:
	enum ConnectionStatus {
		Connected,
		Disconnected,
		CheckingNewConnection
	};

	GlobalSessionService();
	virtual ~GlobalSessionService();

	//! @brief Connects to the Global Session Service at the given URL.
	//! Will change the connection status to CheckingNewConnection and start a health check thread.
	//! The LSS information will be updated on success.
	//! @warning This function will block until the connection is established or fails.
	//! @param _url Global Directory Service url.
	bool connect(const std::string& _url);

	bool isConnected();

	bool confirmSession(const std::string& _sessionId, const std::string& _userName);

	void startHealthCheck();

	void stopHealthCheck();

	GSSRegistrationInfo getRegistrationResult();

private:
	void healthCheck(void);

	std::mutex          m_mutex;
	std::atomic_bool    m_healthCheckRunning;

	ConnectionStatus    m_connectionStatus;
	std::thread*        m_workerThread;
	GSSRegistrationInfo m_registrationResult;
};