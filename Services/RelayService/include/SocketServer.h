
#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslError>

class QWebSocketServer;
class QWebSocket;

#include <string>

Q_DECLARE_METATYPE(const char*);
Q_DECLARE_METATYPE(char*);

class SocketServer : public QObject
{
	Q_OBJECT
public:
	SocketServer(std::string socketIP, unsigned int socketPort);
	~SocketServer();

	std::string sendWSMessage(const std::string operation, const std::string senderIP, const std::string jsonData);
	static bool sendHttpRequest(const std::string &operation, const std::string &url, const std::string &jsonData, std::string &response);

public slots:
	QString performAction(const char *json, const char *senderIP);
	void queueAction(const char *json, const char *senderIP);
	void deallocateData(const char *data);

Q_SIGNALS:
	void closed();

private Q_SLOTS:
	void onNewConnection();
	void processMessage(QString message);
	void socketDisconnected();
	void onSslErrors(const QList<QSslError> &errors);

private:
	void processMessages(void);
	void shutdown();

	QWebSocketServer *m_pWebSocketServer;
	QList<QWebSocket *> m_clients;
	std::string responseText;
	bool responseReceived;
};

#endif //SOCKETSERVER_H
