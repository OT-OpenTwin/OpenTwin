#pragma once
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/ReturnMessage.h"

#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>
#include <QtCore/qobject.h>


#include <atomic>
#include <chrono>

class ConnectionManager {
	
public:	
	ConnectionManager(const std::string& serverName);
	~ConnectionManager();

	bool connectToSubprocess();
	std::string sendRequest(std::string& message);
	std::string receiveResponse();

private:
	std::string m_serverName;
	QLocalSocket* m_socket = nullptr;
};