// @otlicense
// File: WebsocketClient.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/LogDispatcher.h"
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

	bool sendMessage(ot::RelayedMessageHandler::MessageType _type, const std::string& _receiverUrl, const std::string& _message, std::string& _response);
	
	void prepareSessionClosing();
	void updateLogFlags(const ot::LogFlags& _flags);

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
	void dispatchExecuteRequest(ot::RelayedMessageHandler::Request& _data);
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

