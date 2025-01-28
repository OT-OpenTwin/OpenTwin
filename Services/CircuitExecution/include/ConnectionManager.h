#pragma once
#include "QtNetwork/qlocalsocket.h"
#include "QtCore/qobject.h"
#include "OTCore/Logger.h"
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

public slots:
	void send(std::string messageType,std::string message);
private slots:
	void receiveResponse();
	void handleError(QLocalSocket::LocalSocketError error);
	void handleDisconnected();
	void sendHealthCheck();

private:
	QLocalSocket* m_socket;
	QTimer* healthCheckTimer;
	NGSpice* m_ngSpice;
	bool waitForHealthcheck;
	void handleActionType(QString _actionType, QJsonArray _data);
	void handleRunSimulation( std::list<std::string> _netlist);
	void sendBackResults(std::map<std::string, std::vector<double>> _results);

	

};