#pragma once

//OpenTwin Header
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/ReturnMessage.h"

//Qt Header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

class QLocalSocket;
class QLocalServer;



class ConnectionManager : public QObject {
	Q_OBJECT
public:	
	ConnectionManager(QObject* parent = (QObject*)nullptr);
	~ConnectionManager();

	void startListen(const std::string& _serverName);

private Q_SLOTS:
	void handleConnection();
	void handleReadyRead();
	void handleDisconnected();
private:
	
	QLocalServer* m_server;
	QLocalSocket* m_socket;



	
};

