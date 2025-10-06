#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

// OpenTwin header
#include "OTSystem/SystemInformation.h"
#include "OTCore/ServiceDebugInformation.h"
#include "OTCommunication/RelayedMessageHandler.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>
#include <QtCore/qbytearray.h>
#include <QtNetwork/qsslerror.h>
#include <QtCore/qtimer.h>

// std header
#include <string>
#include <thread>
#include <atomic>

class QWebSocketServer;
class QWebSocket;

Q_DECLARE_METATYPE(const char*)

class SocketServer : public QObject
{
	Q_OBJECT
public:
	static SocketServer& instance();

	bool startServer();

	void startSessionServiceHealthCheck(const std::string& _lssUrl);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setWebsocketIp(const std::string& _ip) { m_websocketIp = _ip; };
	const std::string& getWebsocketIp() const { return m_websocketIp; };

	void setWebsocketPort(unsigned int _port) { m_websocketPort = _port; };
	unsigned int getWebsocketPort() const { return m_websocketPort; };

	void setRelayUrl(const std::string& _url) { m_relayUrl = _url; };
	const std::string& getRelayUrl() const { return m_relayUrl; };

	void setServiceId(ot::serviceID_t _id) { m_serviceId = _id; };
	ot::serviceID_t getServiceId() const { return m_serviceId; };

	ot::ServiceDebugInformation getServiceDebugInformation() const;

Q_SIGNALS:
	void responseReceived();
	void connectionClosed();

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
	void messageReceived(const QString& _message);
	void slotProcessMessage(const QString& _message);
	void socketDisconnected();
	void onSslErrors(const QList<QSslError> &errors);
	void slotSocketClosed();
	void keepAlive() const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	SocketServer();
	~SocketServer();

	void shutdown();
	bool sendQueueWSMessage(const std::string& _senderIP, const std::string& _jsonData);
	bool sendProcessWSMessage(const std::string& _senderIP, const std::string& _jsonData, std::string& _response);

	bool relayToHttp(const ot::RelayedMessageHandler::Request& _request, std::string& _response);

	void lssHealthCheckWorker();

	std::string getSystemInformation();

	ot::serviceID_t m_serviceId;
	std::string m_websocketIp;
	unsigned int m_websocketPort;
	std::string m_relayUrl;

	ot::RelayedMessageHandler m_messageHandler;

	QWebSocketServer* m_pWebSocketServer;
	QWebSocket* m_client;
	ot::SystemInformation m_systemLoad;
	std::chrono::system_clock::time_point m_lastReceiveTime;
	QTimer* m_keepAliveTimer;

	std::string m_lssUrl;
	std::atomic_bool m_lssHealthCheckRunning;
	std::thread* m_lssHealthCheckThread;
};

#endif //SOCKETSERVER_H
