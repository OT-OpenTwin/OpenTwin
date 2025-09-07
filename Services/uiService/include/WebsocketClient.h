#pragma once

// OpenTwin header
#include "OTCommunication/RelayedMessageHandler.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtWebSockets/qwebsocket.h>
#include <QtNetwork/qsslerror.h>

// std header
#include <list>
#include <string>

class WebsocketClient : public QObject
{
	Q_OBJECT
public:
	WebsocketClient(const std::string& _socketUrl);
	~WebsocketClient();

	bool sendMessage(bool _queue, const std::string& _receiverUrl, const std::string& _message, std::string& _response);
	
	void prepareSessionClosing();

Q_SIGNALS:
	void connectionClosed();
	void responseReceived();

private Q_SLOTS:
	void slotConnected();
	void slotMessageReceived(const QString& _message);
	void slotSocketDisconnected();
	void slotSslErrors(const QList<QSslError>& _errors);
	void slotProcessMessageQueue();

private:
	void processMessages();	
	void dispatchQueueRequest(ot::RelayedMessageHandler::Request& _data);

	bool ensureConnection();
	void queueBufferProcessingIfNeeded();
	bool anyWaitingForResponse();

	QWebSocket m_webSocket;
	QUrl m_url;
	bool m_isConnected;

	ot::RelayedMessageHandler m_messageHandler;

	bool m_bufferHandlingRequested;
	bool m_currentlyProcessingQueuedMessage;
	std::list<ot::RelayedMessageHandler::Request> m_newRequests; //! @brief New commands that arrived while processing a queued message.
	std::list<ot::RelayedMessageHandler::Request> m_currentRequests; //! @brief Buffer of commands that need to be processed after the current message handling.
	
	bool m_sessionIsClosing;
	bool m_unexpectedDisconnect;
};

