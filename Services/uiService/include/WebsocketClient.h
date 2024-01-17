#pragma once

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
	void processMessages(void);	
	void sendExecuteOrQueueMessage(QString message);
	bool ensureConnection(void);

	QWebSocket m_webSocket;
	QUrl m_url;
	bool isConnected;
	std::map<std::string, bool> waitingForResponse;
	bool currentlyProcessingQueuedMessage;
	std::string responseText;
	std::list<QString> commandQueue;
	bool sessionIsClosing;
};

