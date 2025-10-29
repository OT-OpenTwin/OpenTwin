// @otlicense

#pragma once
#include "QtNetwork/qlocalsocket.h"
#include "QtCore/qobject.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ServiceLogNotifier.h"

class NGSpice;

class ConnectionManager : public QObject {
	Q_OBJECT
public:

	enum RequestType {
		SendResults,
		Message,
		Error
	};
	static QString toString(RequestType _type);

	ConnectionManager(QObject* parent = (QObject*)nullptr);
	~ConnectionManager();

	void connectToCircuitSimulatorService(const QString& serverName);
	void sendBackResults(std::map<std::string, std::vector<double>>& _results);
public slots:
	void sendMessage(std::string messageType,std::string message);
private slots:
	void receiveResponse();
	void handleError(QLocalSocket::LocalSocketError error);
	void handleDisconnected();
	void sendHealthCheck();
	void handleConnected();

private:
	QLocalSocket* m_socket;
	QTimer* healthCheckTimer;
	bool waitForHealthcheck;
	void handleActionType(QString _actionType, QJsonArray _data);
	void handleRunSimulation( std::list<std::string> _netlist);
	void sendJson(QByteArray _data);
	

	

};