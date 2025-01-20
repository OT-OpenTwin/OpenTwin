#pragma once


//Qt Header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

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
	void queueRequest(RequestType _type, const std::list<std::string>& _data);


	// Json Functions
	bool isSingleJsonObject(const QByteArray& data);
	bool isJsonArray(const QByteArray& data);
	bool isMixed(const QByteArray& data);
	int findEndOfJsonObject(const QByteArray& data);
	QList<QJsonObject> handleMultipleJsonObjects(const QByteArray& jsonStram);
	void send(std::string messageType, std::string message);
private Q_SLOTS:
	void handleConnection();
	void handleReadyRead();
	void handleDisconnected();
	void handleQueueRequest(RequestType _type, std::list<std::string> _data);

private:
	
	QLocalServer* m_server;
	QLocalSocket* m_socket;
	QByteArray m_netlist;

	void handleWithJson(const QJsonObject& data);
	void handleMessageType(QString& _actionType, const QJsonValue& _data);


	
};

