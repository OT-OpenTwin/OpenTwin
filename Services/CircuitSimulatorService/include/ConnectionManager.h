#pragma once

#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/ReturnMessage.h"

#include <QtNetwork/qlocalsocket.h>
#include <QtNetwork/qlocalserver.h>
#include <QtCore/qobject.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>



#include <atomic>
#include <chrono>

class ConnectionManager : public QObject {
	Q_OBJECT
public:	
	ConnectionManager(QObject* parent = (QObject*)nullptr);
	~ConnectionManager();

	void startListen(std::string serverName);

	QLocalSocket* getSocket();
	QLocalServer& getServer();

	void connectWithSubprocess();

	void setServerName(std::string serverName);

private:
	std::string m_serverName;
	QLocalSocket* m_socket = nullptr;
	QLocalServer m_server;

	const int m_timeoutServerConnect =  50000;
	const int m_timeoutSendingMessage = m_timeoutServerConnect;

	void SocketErrorOccured(std::string& message);
};

