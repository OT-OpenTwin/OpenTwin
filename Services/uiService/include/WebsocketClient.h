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
	enum MessageType {
		EXECUTE,
		QUEUE
	};

	WebsocketClient(const std::string& _socketUrl);
	~WebsocketClient();

	void sendMessage(const std::string& _receiverUrl, MessageType _messageType, const std::string& _message, std::string& _response);
	
	void prepareSessionClosing();

Q_SIGNALS:
	void closed();
	void responseReceived();

private Q_SLOTS:
	void slotConnected();
	void slotMessageReceived(const QString& _message);
	void slotSocketDisconnected();
	void slotSslErrors(const QList<QSslError>& _errors);
	void slotProcessMessageQueue();

private:
	struct CommandData {
		std::string action;
		std::string senderIp;
		std::string data;
	};

	void processMessages();	
	void dispatchQueueAction(CommandData& _data);

	bool ensureConnection();
	void queueBufferProcessingIfNeeded();
	bool anyWaitingForResponse() const;
	bool isWaitingForResponse(const std::string& _senderIP) const;

	QWebSocket m_webSocket;
	QUrl m_url;
	bool m_isConnected;
	bool m_bufferHandlingRequested;
	std::map<std::string, bool> m_waitingForResponse;
	bool m_currentlyProcessingQueuedMessage;
	std::string m_responseText;
	
	std::list<CommandData> m_newQueueCommands; //! @brief New commands that arrived while processing a queued message.
	std::list<CommandData> m_commandsBuffer; //! @brief Buffer of commands that need to be processed after the current message handling.
	
	bool m_sessionIsClosing;
	bool m_unexpectedDisconnect;
};

