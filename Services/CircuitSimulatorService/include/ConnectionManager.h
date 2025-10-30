// @otlicense
// File: ConnectionManager.h
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


// Qt Header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qtimer.h>

class QLocalSocket;
class QLocalServer;



class ConnectionManager : public QObject {
	Q_OBJECT
public:
	enum RequestType {
		ExecuteNetlist,
		Message,
		Error,
		Disconnect
	};

	static QString toString(RequestType _type);
	ConnectionManager(QObject* parent = (QObject*)nullptr);
	~ConnectionManager();


	void startListen(const std::string& _serverName);
	bool sendNetlist();
	bool waitForClient(void);

	
	void send(std::string messageType, std::string message);

	// Setter
	void setNetlist(RequestType _type,std::list<std::string>& _netlist);

private Q_SLOTS:
	void handleConnection();
	void handleReadyRead();
	void handleDisconnected();
	void sendHealthcheck();

private:
	
	QLocalServer* m_server;
	QLocalSocket* m_socket;
	QByteArray m_netlist;
	QTimer* healthCheckTimer;
	bool waitForHealthcheck;
	void handleMessageType(QString& _actionType, const QJsonValue& _data);


	
};

