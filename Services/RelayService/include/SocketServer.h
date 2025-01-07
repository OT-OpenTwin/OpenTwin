#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

// OpenTwin header
#include "OTSystem/SystemInformation.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>
#include <QtCore/qbytearray.h>
#include <QtNetwork/qsslerror.h>
#include <QtCore/qtimer.h>

// std header
#include <string>

class QWebSocketServer;
class QWebSocket;

Q_DECLARE_METATYPE(const char*);
Q_DECLARE_METATYPE(char*);

class SocketServer : public QObject
{
	Q_OBJECT
public:
	static SocketServer& instance(void);

	bool startServer(void);

	bool sendHttpRequest(const std::string& operation, const std::string& url, const std::string& jsonData, std::string& response);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setWebsocketIp(const std::string& _ip) { m_websocketIp = _ip; };
	const std::string& getWebsocketIp(void) const { return m_websocketIp; };

	void setWebsocketPort(unsigned int _port) { m_websocketPort = _port; };
	unsigned int getWebsocketPort(void) const { return m_websocketPort; };

	void setRelayUrl(const std::string& _url) { m_relayUrl = _url; };
	const std::string& getRelayUrl(void) const { return m_relayUrl; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public: Slots

public Q_SLOTS:
	QString performAction(const char *json, const char *senderIP);
	void queueAction(const char *json, const char *senderIP);
	void deallocateData(const char *data);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void onNewConnection(void);
	void processMessage(QString message);
	void socketDisconnected(void);
	void onSslErrors(const QList<QSslError> &errors);
	void slotSocketClosed(void);
	void keepAlive();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	SocketServer();
	~SocketServer();

	void processMessages(void);
	void shutdown(void);
	void sendQueueWSMessage(const std::string operation, const std::string senderIP, const std::string jsonData);
	std::string sendProcessWSMessage(const std::string operation, const std::string senderIP, const std::string jsonData);
	std::string getSystemInformation(void);

	std::string m_websocketIp;
	unsigned int m_websocketPort;
	std::string m_relayUrl;
	
	QWebSocketServer *m_pWebSocketServer;
	QList<QWebSocket *> m_clients;
	std::string responseText;
	bool responseReceived;
	ot::SystemInformation m_systemLoad;
	std::chrono::system_clock::time_point m_lastReceiveTime;
	QTimer* m_keepAliveTimer;
};

#endif //SOCKETSERVER_H
