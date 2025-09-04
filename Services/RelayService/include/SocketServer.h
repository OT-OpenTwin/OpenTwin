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

Q_DECLARE_METATYPE(const char*)

class SocketServer : public QObject
{
	Q_OBJECT
public:
	static SocketServer& instance();

	bool startServer();

	bool sendHttpRequest(const std::string& _operation, const std::string& _url, const std::string& _jsonData, std::string& _response);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setWebsocketIp(const std::string& _ip) { m_websocketIp = _ip; };
	const std::string& getWebsocketIp() const { return m_websocketIp; };

	void setWebsocketPort(unsigned int _port) { m_websocketPort = _port; };
	unsigned int getWebsocketPort() const { return m_websocketPort; };

	void setRelayUrl(const std::string& _url) { m_relayUrl = _url; };
	const std::string& getRelayUrl() const { return m_relayUrl; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public: Slots

public Q_SLOTS:
	QString performAction(const char * _json, const char * _senderIP);
	void queueAction(const char * _json, const char * _senderIP);
	void deallocateData(const char * _data);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void onNewConnection();
	void processMessage(QString message);
	void socketDisconnected();
	void onSslErrors(const QList<QSslError> &errors);
	void slotSocketClosed();
	void keepAlive() const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	SocketServer();
	~SocketServer();

	void processMessages();
	void shutdown();
	void sendQueueWSMessage(const std::string& _operation, const std::string& _senderIP, const std::string& _jsonData);
	std::string sendProcessWSMessage(const std::string& _operation, const std::string& _senderIP, const std::string& _jsonData);
	std::string getSystemInformation();

	std::string m_websocketIp;
	unsigned int m_websocketPort;
	std::string m_relayUrl;
	
	QWebSocketServer* m_pWebSocketServer;
	QList<QWebSocket *> m_clients;
	std::string m_responseText;
	bool m_responseReceived;
	ot::SystemInformation m_systemLoad;
	std::chrono::system_clock::time_point m_lastReceiveTime;
	QTimer* m_keepAliveTimer;
};

#endif //SOCKETSERVER_H
