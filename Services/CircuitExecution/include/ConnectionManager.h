#pragma once
#include "QtNetwork/qlocalsocket.h"
#include "QtCore/qobject.h"
#include "OTCore/Logger.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ServiceLogNotifier.h"

class ConnectionManager : public QObject {
	Q_OBJECT
public: 
	ConnectionManager(QObject* parent = (QObject*)nullptr);
	~ConnectionManager() = default;

	void connectToCircuitSimulatorService(const QString& serverName);
private slots:
	void receiveResponse();
	void sendHello();
	void handleError(QLocalSocket::LocalSocketError error);
	void handleDisconnected();
private:
	QLocalSocket* m_socket;


	

};