#pragma once

#include "OTCore/ServiceBase.h"

#include <string>
#include <list>

class SessionService;

class GlobalDirectoryService : ot::ServiceBase {
public:
	enum ConnectionStatus {
		Connected,
		Disconnected,
		CheckingNewConnection
	};

	GlobalDirectoryService(SessionService * _sessionService);
	virtual ~GlobalDirectoryService();

	void connect(const std::string& _url);
	bool isConnected(void) const;
	inline ConnectionStatus connectionStatus(void) const { return m_connectionStatus; }

	bool requestToStartService(const ot::ServiceBase& _serviceInformation, const std::string& _sessionID);
	bool requestToStartServices(const std::list<ot::ServiceBase>& _serviceInformation, const std::string& _sessionID);
	bool requestToStartRelayService(const std::string& _sessionID, std::string& _websocketURL, std::string& _relayServiceURL);
	void notifySessionClosed(const std::string& _sessionID);

private:
	void healthCheck(void);

	SessionService *	m_sessionService;
	ConnectionStatus	m_connectionStatus;
	bool				m_isShuttingDown;

	GlobalDirectoryService() = delete;
	GlobalDirectoryService(GlobalDirectoryService&) = delete;
	GlobalDirectoryService& operator = (GlobalDirectoryService&) = delete;
};