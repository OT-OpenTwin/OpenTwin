#include "WebsocketClient.h"

#include <QEventLoop>
#include <QFile>
#include <QCoreApplication>
#include <iostream>
#include <thread>

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"

// SSL
#include <QtCore/QFile>
#include <QProcessEnvironment>


extern "C"
{
	_declspec(dllexport) const char *performAction(const char *json, const char *senderIP);
	_declspec(dllexport) const char *queueAction(const char *json, const char *senderIP);
	_declspec(dllexport) void deallocateData(const char *data);
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

WebsocketClient::WebsocketClient(const std::string &socketUrl) :
	QObject(nullptr),
	isConnected(false),
	currentlyProcessingQueuedMessage(false),
	sessionIsClosing(false)
{
	std::string wsUrl = "wss://" + socketUrl;
	// TODO: REMOVE THIS AFTER Qt UPDATE
	replace(wsUrl, "127.0.0.1", "localhost");
	m_url = QUrl(wsUrl.c_str());

	QString caStr;
	std::string message;
	const QString developerBasePath = QProcessEnvironment::systemEnvironment().value("OPENTWIN_DEV_ROOT", "");
	
	if (developerBasePath == "")
	{
		caStr = QCoreApplication::applicationDirPath() + "\\Certificates\\ca.pem";
		message = "Using deployment root certificate file: " + caStr.toStdString();
	}
	else
	{
		caStr = developerBasePath + "\\Deployment\\Certificates\\ca.pem";
		message = "Using development root certificate file: " + caStr.toStdString();
	}

	OT_LOG_D(message);
	std::cout << message << std::endl;

	// Check whether local cert file ca.pem exists
	if (!QFile::exists(caStr))
	{
		throw std::exception("Root certificate not found.");
	}

	// SSL/ TLS Configuration
	QSslConfiguration sslConfiguration;
		
	QFile caCertFile(caStr);
	caCertFile.open(QIODevice::ReadOnly);
	QSslCertificate caCert(&caCertFile, QSsl::Pem);
	caCertFile.close();

	
	QList<QSslCertificate> caCerts = sslConfiguration.caCertificates();
	caCerts.append(caCert);
	sslConfiguration.setCaCertificates(caCerts);
	
	m_webSocket.setSslConfiguration(sslConfiguration);

	connect(&m_webSocket, &QWebSocket::connected, this, &WebsocketClient::onConnected);
	connect(&m_webSocket, &QWebSocket::disconnected, this, &WebsocketClient::socketDisconnected);

	connect(&m_webSocket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),
		this, &WebsocketClient::onSslErrors);

	m_webSocket.open(QUrl(m_url));
	OT_LOG_D("Websocket opened with state: " + stateAsString(m_webSocket.state()));
}

WebsocketClient::~WebsocketClient()
{
	m_webSocket.close();

	processMessages();
}


void WebsocketClient::onSslErrors(const QList<QSslError> &errors)
{
	//Q_UNUSED(errors);

	foreach(QSslError err, errors)
	{
		OT_LOG_E(err.errorString().toStdString());
	}
	// WARNING: Never ignore SSL errors in production code.
	// The proper way to handle self-signed certificates is to add a custom root
	// to the CA store.

	// TODO: REMOVE THIS AFTER Qt UPDATE
	m_webSocket.ignoreSslErrors();
}

void WebsocketClient::onConnected()
{
	OT_LOG_D("Client connected");

	connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebsocketClient::onMessageReceived);
	isConnected = true;
}

void WebsocketClient::socketDisconnected()
{
	if (!isConnected) return; // This message might be sent on an unsuccessful connection attempt (when the relay server is not yet ready). In this case, we can 
							  // safely ignore this message.

	OT_LOG_D("Relay server disconnected on websocket");
	isConnected = false;

	if (!sessionIsClosing)
	{
		// This is an unexpected disconnect of the relay service -> we need to close the session
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceEmergencyShutdown, doc.GetAllocator()), doc.GetAllocator());
		currentlyProcessingQueuedMessage = true;
		queueAction(doc.toJson().c_str(), "");
	}
}

void WebsocketClient::onMessageReceived(QString message)
{
	// Get the action from the message
	int index1 = message.indexOf('\n');
	int index2 = message.indexOf('\n', index1 + 1);

	QString action = message.left(index1);
	QString senderIP = message.mid(index1 + 1, index2 - index1 - 1);

	// Process the action
	if (action == "response")
	{
		responseText = message.mid(index2 + 1).toStdString();
		waitingForResponse[senderIP.toStdString()] = false;
	}
	else if (action == "execute")
	{
		if (currentlyProcessingQueuedMessage || waitingForResponse[senderIP.toStdString()])
		{
			commandQueue.push_back(message);
		}
		else
		{
			sendExecuteOrQueueMessage(message);
		}
	}
	else if (action == "queue")
	{
		if (currentlyProcessingQueuedMessage || waitingForResponse[senderIP.toStdString()])
		{
			commandQueue.push_back(message);
		}
		else
		{
			sendExecuteOrQueueMessage(message);
		}
	}
}

void WebsocketClient::finishedProcessingQueuedMessage(void)
{
	assert(currentlyProcessingQueuedMessage);
	currentlyProcessingQueuedMessage = false;

	if (!commandQueue.empty())
	{
		QString message = commandQueue.front();
		commandQueue.pop_front();

		onMessageReceived(message);
	}
}

void WebsocketClient::sendResponse(const std::string &message)
{
	// Make sure we are connected
	if (!ensureConnection()) return;

	// Send the reponse
	m_webSocket.sendTextMessage(message.c_str());
}

void WebsocketClient::sendMessage(const std::string &message, std::string &response)
{
	size_t index1 = message.find('\n');
	size_t index2 = message.find('\n', index1 + 1);

	std::string senderIP = message.substr(index1 + 1, index2 - index1 - 1);

	// Make sure we are connected
	if (!ensureConnection())
	{
		OT_LOG_D("Sending message via websocket client failed, because not connected.");
		return;
	}

	// Now send our message	
	waitingForResponse[senderIP] = true;
	OT_LOG_D("Sending websocket message");
	m_webSocket.sendTextMessage(message.c_str());

	// Wait for the reponse
	OT_LOG_D("Starting to wait for response");
	while (waitingForResponse[senderIP])
	{
		processMessages();
	}
	OT_LOG_D("Finished waitinf for a response");
	// We have received a response and return the text
	response = responseText;
}

void WebsocketClient::processMessages(int maxTime) 
{
	QEventLoop eventLoop;		
	if (maxTime == 0)
	{
		eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
	}
	else
	{
		eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents,maxTime);
	}
}

bool WebsocketClient::ensureConnection(void)
{
	if (!isConnected && sessionIsClosing) 
	{
		OT_LOG_D("WebsocketClient is not connected and the session is closing");
		return false;
	}
	int maxWaitingTime = 1000; //in ms
	
	auto socketState = m_webSocket.state();
	if (!isConnected)
	{
		OT_LOG_D("Websocket not connected. Socket state: " + socketState);
	}
	while (!isConnected)
	{
		if (socketState == QAbstractSocket::UnconnectedState)
		{
			// The relay service was probably not running when the connection was tried to establish
			// Try to connect again
			m_webSocket.open(QUrl(m_url));
		}
		processMessages(maxWaitingTime);
	}
	assert(isConnected);

	return isConnected;
}

std::string WebsocketClient::stateAsString(QAbstractSocket::SocketState state)
{
	if (state == QAbstractSocket::UnconnectedState)
	{
		return "The socket is not connected.";
	}
	else if(state == QAbstractSocket::HostLookupState)
	{
		return "The socket is performing a host name lookup.";
	}
	else if (state == QAbstractSocket::ConnectingState)
	{
		return "The socket has started establishing a connection.";
	}
	else if (state == QAbstractSocket::ConnectedState)
	{
		return "A connection is established.";
	}
	else if (state == QAbstractSocket::BoundState)
	{
		return "The socket is bound to an address and port.";
	}
	else if (state == QAbstractSocket::ClosingState)
	{
		return "The socket is about to close(data may still be waiting to be written).";
	}
	else
	{
		assert(QAbstractSocket::ListeningState); 
		return "For internal use only.";
	}
}

void WebsocketClient::sendExecuteOrQueueMessage(QString message)
{
	// We have received a message through the websocket and need to forward it to the standard
	// messaging system for dispatching received messages.

	int index1 = message.indexOf('\n');
	int index2 = message.indexOf('\n', index1 + 1);

	QString action = message.left(index1);
	QString senderIP = message.mid(index1 + 1, index2 - index1 - 1);
	QString jsonData = message.mid(index2 + 1);

	const char *response = nullptr;
	
	if (action == "execute")
	{
		response = performAction(jsonData.toStdString().c_str(), senderIP.toStdString().c_str());
	}
	else if (action == "queue")
	{
		currentlyProcessingQueuedMessage = true;
		response = queueAction(jsonData.toStdString().c_str(), senderIP.toStdString().c_str());
	}
	else
	{
		assert(0); // Unknown action.
		return;
	}

	// The message was processed and we need to relay the reponse
	std::string returnMessage = "response\n";

	if (response != nullptr)
	{
		returnMessage += response;
		deallocateData(response);
	}

	sendResponse(returnMessage);
}