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
	WebsocketClient(const std::string& _socketUrl);
	~WebsocketClient();

	void sendMessage(const std::string& _message, std::string& _response);
	void sendResponse(const std::string& _message);

	void finishedProcessingQueuedMessage(void);

	void prepareSessionClosing(void);

Q_SIGNALS:
	void closed();

private Q_SLOTS:
	void slotConnected();
	void slotMessageReceived(const QString& _message);
	void slotSocketDisconnected();
	void slotSslErrors(const QList<QSslError>& _errors);
	void slotProcessMessageQueue(void);

private:
	void handleMessageReceived(const QString& _message, bool _isExternalMessage);
	void processMessages(void);	
	void sendExecuteOrQueueMessage(QString message);
	bool ensureConnection(void);
	void queueMessageProcessingIfNeeded(void);
	bool anyWaitingForResponse(void) const;

	QWebSocket m_webSocket;
	QUrl m_url;
	bool m_isConnected;
	std::map<std::string, bool> m_waitingForResponse;
	bool m_currentlyProcessingQueuedMessage;
	std::string m_responseText;
	std::list<QString> m_commandQueue;
	bool m_sessionIsClosing;
	bool m_unexpectedDisconnect;
};

