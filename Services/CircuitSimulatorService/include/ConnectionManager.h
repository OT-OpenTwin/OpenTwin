#pragma once
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/ReturnMessage.h"

#include <QtNetwork/qlocalsocket.h>
#include <QtNetwork/qlocalserver.h>


#include <atomic>
#include <chrono>

class ConnectionManager {
	
public:	
	ConnectionManager();
	~ConnectionManager();

	void startListen(std::string serverName);

	QLocalSocket* getSocket();
	QLocalServer& getServer();

	
private:
	std::string m_serverName;
	QLocalSocket* m_socket = nullptr;
	QLocalServer m_server;
};