// @otlicense

#pragma once


// Qt Header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qtimer.h>

class QLocalSocket;
class QLocalServer;



class ConnectionManager : public QObject {
	Q_OBJECT
public:
	enum RequestType {
		ExecuteNetlist,
		Message,
		Error,
		Disconnect
	};

	static QString toString(RequestType _type);
	ConnectionManager(QObject* parent = (QObject*)nullptr);
	~ConnectionManager();


	void startListen(const std::string& _serverName);
	bool sendNetlist();
	bool waitForClient(void);

	
	void send(std::string messageType, std::string message);

	// Setter
	void setNetlist(RequestType _type,std::list<std::string>& _netlist);

private Q_SLOTS:
	void handleConnection();
	void handleReadyRead();
	void handleDisconnected();
	void sendHealthcheck();

private:
	
	QLocalServer* m_server;
	QLocalSocket* m_socket;
	QByteArray m_netlist;
	QTimer* healthCheckTimer;
	bool waitForHealthcheck;
	void handleMessageType(QString& _actionType, const QJsonValue& _data);


	
};

