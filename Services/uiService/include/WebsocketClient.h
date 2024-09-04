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

	void isClosing(void) { m_sessionIsClosing = true; }

Q_SIGNALS:
	void closed();

private Q_SLOTS:
	void slotConnected();
	void slotMessageReceived(QString message);
	void slotSocketDisconnected();
	void slotSslErrors(const QList<QSslError> &errors);

private:
	void processMessages(void);	
	void sendExecuteOrQueueMessage(QString message);
	bool ensureConnection(void);

	QWebSocket m_webSocket;
	QUrl m_url;
	bool m_isConnected;
	std::map<std::string, bool> m_waitingForResponse;
	bool m_currentlyProcessingQueuedMessage;
	std::string m_responseText;
	std::list<QString> m_commandQueue;
	bool m_sessionIsClosing;
};

