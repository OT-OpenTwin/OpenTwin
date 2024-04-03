#pragma once
#include <atomic>

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>

#include <string>
#include <list>

class WebsocketClient : public QObject
{
	Q_OBJECT
public:
	WebsocketClient(const std::string& socketUrl);
	~WebsocketClient();

	void sendMessage(const std::string &message, std::string &response);
	void sendResponse(const std::string &message);

	void finishedProcessingQueuedMessage(void);

	void isClosing(void) { sessionIsClosing = true; }

Q_SIGNALS:
	void closed();

private Q_SLOTS:
	void onConnected();
	void onMessageReceived(QString message);
	void socketDisconnected();
	void onSslErrors(const QList<QSslError> &errors);

private:
	void processMessages();	
	void sendExecuteOrQueueMessage(QString message);
	bool ensureConnection(void);

	std::string stateAsString(QAbstractSocket::SocketState state);

	QWebSocket m_webSocket;
	QUrl m_url;
	std::atomic_bool isConnected;
	std::map<std::string, std::atomic_bool> waitingForResponse;
	std::atomic_bool currentlyProcessingQueuedMessage;
	std::string responseText;
	std::list<QString> commandQueue;
	std::atomic_bool sessionIsClosing;
};

