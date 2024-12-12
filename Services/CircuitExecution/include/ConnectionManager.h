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
		ExecuteNetlist,
		Message
	};

	

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
	NGSpice* m_ngSpice;
	void handleActionType(QString _actionType, QJsonArray _data);
	void handleRunSimulation( std::list<std::string> _netlist);
	void sendBackResults(std::map<std::string, std::vector<double>> _results);

	

};